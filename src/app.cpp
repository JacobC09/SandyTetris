#include <cmath>
#include "app.h"
#include "assets.h"
#include "pixelFont.h"
#include "debug.h"
#include "shapes.h"

Vector2 IndexToPos(int index, int sideLength) {
    return Vector2 {
        (float) (index % sideLength), 
        (float) std::floor(index / sideLength)
    }; 
}

// Load the application
void Application::Load() {
    InitWindow(screenWidth, screenHeight, "Desert Tetris");
    InitAudioDevice();
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);
    LoadAssets();

    Shader &heatShader = GetShader(Shaders::Heat);

    iResoluationLoc = GetShaderLocation(heatShader, "iResolution");
    iTimeLoc = GetShaderLocation(heatShader, "iTime");
    time = 0;

    float res[2] = {(float) screenWidth, (float) screenHeight};

    SetShaderValue(heatShader, iResoluationLoc, res, SHADER_UNIFORM_VEC2);
    SetShaderValue(heatShader, iTimeLoc, 0, SHADER_UNIFORM_FLOAT);

    game = Game(this);
    game.Load();
    game.NewGame();
}

// Run the application
void Application::Run() {
    while (!WindowShouldClose()) {
        if (!IsSoundPlaying(GetSound(Sounds::gerudoValley)) && settings.music)
            PlaySound(GetSound(Sounds::gerudoValley));

        time += 1;
        float value[1] = {(float) time / 60};
        SetShaderValue(GetShader(Shaders::Heat), iTimeLoc, value, SHADER_UNIFORM_FLOAT);

        BeginDrawing();
            
        switch (state) {
            case Application::States::Running:
                game.Update();
                break;
        }

        // if (IsKeyPressed(KEY_SPACE)) {
        //     transitions.push_back(new BlockFadeTransition(darkBrown, 30, 40));
        // }

        for (int i = transitions.size() - 1; i > -1; i--) {
            if (transitions[i]->isFinished()) {
                transitions.erase(transitions.begin() + i);
            } else {
                transitions[i]->Draw();
            }
        }

        EndDrawing();
    }
}

// Unload the applicaiton
void Application::Unload() {
    UnloadAssets();
    CloseWindow();
}

// Load Game
void Game::Load() {
    simulation = Simulation(boardWidth * tileSize, boardHeight * tileSize);
    boardTex = LoadRenderTexture(simulation.width, simulation.height);
    bgAnimation = Timer {240};
    blocksImg = LoadImageFromTexture(GetTexture(Textures::blocks));
}

// Create a new game
void Game::NewGame() {
    simulation.Reset();
    bgAnimation.reset();
    sinceSandUpdate = 0;
    shapeMoveSpeed = 2;
    shapeFallSpeed = 1;
    gameOver = false;
    connectionAnim = ConnectionAnim {{timer: 0, active: false}, positions: {}};

    boardRect = {
        screenWidth / 2 - boardWidth * tileSize * scale / 2, 
        screenHeight / 2 - boardHeight * tileSize * scale / 2, 
        boardWidth * tileSize * scale, 
        boardHeight * tileSize * scale
    };

    SpawnShape();
}

// Update game
void Game::Update() {
    if (!connectionAnim.active) {
        if (++sinceSandUpdate > 1) {
            simulation.Step();
            sinceSandUpdate = 0;
        }

        if (currentShape.type == -1)
            SpawnShape();

        MoveShape();
        RotateShape();
        FindConnectedSand();
    }

    DrawBg();

    BeginTextureMode(boardTex);
        ClearBackground(ColorAlpha(BLACK, 0.9));
        DrawSandToTex();

        if (currentShape.type != -1)
            DrawShape(currentShape, {std::floor(cShapePos.x), std::floor(cShapePos.y)}, 1);
        
        if (connectionAnim.active) {
            UpdateConnectAnim();
        }
    
    EndTextureMode();
    DrawTexturePro(boardTex.texture, {0, 0, (float) boardTex.texture.width, (float) -boardTex.texture.height}, boardRect, {0, 0}, 0, WHITE);

    if (gameOver) {
        NewGame();
        app->transitions.push_back(new BlockFadeTransition(darkBrown, 40, 40));
    }
}

void Game::DrawBg() {
    // Draw Background
    BeginShaderMode(GetShader(Shaders::Heat));
        Texture2D &texture = (bgAnimation.timer < bgAnimation.total / 2) ? GetTexture(Textures::desertBg2) : GetTexture(Textures::desertBg1);
        DrawTexturePro(texture, {0, 0, (float) texture.width, (float) texture.height}, {0, 0, (float) screenWidth, (float) screenHeight}, {0, 0}, 0, WHITE);
    EndShaderMode();

    
    bgAnimation.update();

    // Draw Board Border
    const int boardBorderThickness = 6;

    DrawRectangleLinesEx({
        boardRect.x - boardBorderThickness, 
        boardRect.y - boardBorderThickness,  boardRect.width + boardBorderThickness * 2, 
        boardRect.height + boardBorderThickness * 2
    }, boardBorderThickness, darkBrown);
}

void Game::DrawSandToTex() {
    for (int y = 0; y < simulation.height; y++) {
        for (int x = 0; x < simulation.width; x++) {
            SandParticle* particle = simulation.GetAt(x, y);
            if (!particle->occupied) continue;
            DrawPixel(x, y, particle->color);
        }
    }
}

void Game::MoveShape() {
    Vector2 movement = {0, 0};

    bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);

    movement.y += down ? shapeFallSpeed * 2 : shapeFallSpeed;

    bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);

    if (right && !left) {
        movement.x += shapeMoveSpeed;
    } else if (left && !right) {
        movement.x -= shapeMoveSpeed;
    }

    CheckShapeCollision(movement);
}

void Game::RotateShape() {
    bool up = IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
    bool down = IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN);

    int totalRotations = (signed) shapeTypes[currentShape.type].rotations.size();
    int oldRotation = currentShape.rotation;

    if (up && !down) {
        if (++currentShape.rotation >= totalRotations)
            currentShape.rotation = 0;
    } else if (up && !down) {
        if (--currentShape.rotation < 0)
            currentShape.rotation = totalRotations - 1;
    }

    if (currentShape.rotation != oldRotation && !IsKeyDown(KEY_SPACE)) {
        if (IsShapeColliding())
            currentShape.rotation = oldRotation;
        else {
            TryToCorrectShape();
        }
    }
}

// Try to move shape out of bounds only it doesn't collide with the simulation
void Game::TryToCorrectShape() {
    Vector2 oldPos = cShapePos;
    int size = shapeTypes[currentShape.type].size;
    const auto bitmap = shapeTypes[currentShape.type].rotations[currentShape.rotation].bitmap;

    for (int i = 0; i < (signed) bitmap.size(); i++) {
        if (!bitmap[i]) continue;

        Vector2 pos = IndexToPos(i, size);
        
        // Border
        int left = cShapePos.x + (pos.x * tileSize);;
        if (left < 0) {
            cShapePos.x -= left;
        } else if (left + tileSize >= simulation.width) {
            cShapePos.x -= (left + tileSize) - simulation.width;
        }
    }

    if (IsShapeColliding())
        cShapePos = oldPos;
}

void Game::SpawnShape() {
    currentShape = NewShape();
    Rectangle shapeRect = GetShapeRect(currentShape);
    
    cShapePos = Vector2 {
        simulation.width / 2 - (shapeRect.x + shapeRect.width / 2) * tileSize,
        -shapeRect.y * tileSize
    };

    if (IsShapeColliding()) {
        gameOver = true;
    }
}

void Game::CheckShapeCollision(Vector2 mouvement) {
    int size = shapeTypes[currentShape.type].size;
    const auto bitmap = shapeTypes[currentShape.type].rotations[currentShape.rotation].bitmap;

    bool hitBottom = false;

    // Left-Right Sand Collision
    if (mouvement.x > 0) {
        float xmouvement = 0;
        while (xmouvement < mouvement.x) {
            float distance = std::min(mouvement.x - xmouvement, 1.0f);
            xmouvement += distance;
            cShapePos.x += distance;

            if (IsShapeColliding()) {
                cShapePos.x -= distance;
                break;
            }
        }

    } else if (mouvement.x < 0) {
        float xmouvement = 0;
        while (xmouvement > mouvement.x) {
            float distance = std::max(mouvement.x - xmouvement, -1.0f);
            xmouvement += distance;
            cShapePos.x += distance;

            if (IsShapeColliding()) {
                cShapePos.x -= distance;
                break;
            }
        }
    }

    // Bottom Sand Mouvement
    if (mouvement.y) {
        float ymouvement = 0;
        while (ymouvement < mouvement.y) {
            float distance = std::min(mouvement.y - ymouvement, 1.0f);
            ymouvement += distance;
            cShapePos.y += distance;

            if (IsShapeColliding()) {
                cShapePos.y -= distance;
                hitBottom = true;
                break;
            }
        }
    }

    // Board Collision
    for (int i = 0; i < (signed) bitmap.size(); i++) {
        if (!bitmap[i]) continue;

        Vector2 pos = IndexToPos(i, size);
        
        // Border
        int left = cShapePos.x + (pos.x * tileSize);;
        if (left < 0) {
            cShapePos.x -= left;
        } else if (left + tileSize >= simulation.width) {
            cShapePos.x -= (left + tileSize) - simulation.width;
        }

        // Bottom
        int bottom = cShapePos.y + (pos.y * tileSize) + tileSize;
        if (bottom >= simulation.height) {
            cShapePos.y -= bottom - simulation.height;
            hitBottom = true;
        }
    }

    if (hitBottom) {
        TurnShapeToSand();
        currentShape.type = -1;
    }
}

void Game::TurnShapeToSand() {
    int size = shapeTypes[currentShape.type].size;
    const auto bitmap = shapeTypes[currentShape.type].rotations[currentShape.rotation].bitmap;

    Rectangle src = {
        (float) currentShape.style * tileSize,
        (float) currentShape.color * tileSize,
        tileSize,
        tileSize
    };

    for (int i = 0; i < (signed) bitmap.size(); i++) {
        if (!bitmap[i]) continue;

        Vector2 pos = IndexToPos(i, size);        
        pos.x *= tileSize;
        pos.y *= tileSize;

        for (int x = 0; x < tileSize; x++) {
            for (int y = 0; y < tileSize; y++) {
                simulation.SetAt(cShapePos.x + pos.x + x, cShapePos.y + pos.y + y, SandParticle {
                    occupied: true,
                    color: GetImageColor(blocksImg, src.x + x, src.y + y),
                    type: currentShape.color
                });
            }
        }
    }
}

// Fix this garbage
void Game::FindConnectedSand() {
    const int totalBorderPositions = 4;
    const Position borderPositions[totalBorderPositions] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // Change all of the particle visited fields to false
    simulation.ResetVisited();

    std::string output;

    for (int y = simulation.height - 1; y > -1; y--) {
        SandParticle* startingParticle = simulation.GetAt(0, y);

        // Only process particles that are occupied
        if (!startingParticle->occupied) continue;
     
        bool connected = false;
        startingParticle->visited = true;

        // Vector of all visited positions
        std::vector<Position> visited;
        visited.reserve(tileSize * tileSize * 4);

        // Vector of positions that have yet to be processed
        std::vector<Position> processQueue;
        processQueue.push_back({0, y});
        visited.push_back({0, y});

        while (processQueue.size()) {
            Position position = processQueue.back();
            processQueue.pop_back();

            // If the current position y is on the left border
            if (position.y < y && position.x == 0) {
                y = position.y;
            }

            // Check borders to see if they have the same type
            for (int i = 0; i < totalBorderPositions; i++) {
                Position newPos = {position.x + borderPositions[i].x, position.y + borderPositions[i].y};
                            
                if (!simulation.ValidPosition(newPos.x, newPos.y)) continue;

                SandParticle* particle = simulation.GetAt(newPos.x, newPos.y);
                if (!particle->occupied || particle->type != startingParticle->type || particle->visited) continue;

                if (newPos.x == simulation.width - 1)
                    connected = true;

                particle->visited = true;
                processQueue.push_back(newPos);
                visited.push_back(newPos);
            }
        }

        if (connected) {
            connectionAnim.positions = visited;
            connectionAnim.active = true;
            connectionAnim.reset();
        }
    }
}

void Game::UpdateConnectAnim() {
    const int waitBeforeFade = 10;
    const int maxFallBackDistance = 16;

    connectionAnim.timer++;
    bool isWhite = connectionAnim.timer % 40 < 20;
    int x = (connectionAnim.timer - waitBeforeFade) * 2;

    for (int i = connectionAnim.positions.size() - 1; i > -1; i--) {
        Position pos = connectionAnim.positions[i];
        
        if (pos.x < x) {
            if (GetRandomValue(0, std::max(maxFallBackDistance - (x - pos.x), 0)) == 0) {
                connectionAnim.positions.erase(connectionAnim.positions.begin() + i);
                simulation.SetAt(pos.x, pos.y, SandParticle{false});
            }
        }

        if (isWhite)
            DrawPixel(pos.x, pos.y, WHITE);
    }

    if (x > simulation.width + maxFallBackDistance) {
        connectionAnim.active = false;
        connectionAnim.positions.clear();
    }
}

bool Game::IsShapeColliding() {
    int size = shapeTypes[currentShape.type].size;
    const auto bitmap = shapeTypes[currentShape.type].rotations[currentShape.rotation].bitmap;

    for (int i = 0; i < (signed) bitmap.size(); i++) {
        if (!bitmap[i]) continue;

        Vector2 pos = IndexToPos(i, size);
        pos.x *= tileSize;
        pos.y *= tileSize;

        for (int y = tileSize - 1; y > -1; y--) {
            for (int x = 0; x < tileSize; x++) {
                SandParticle* particle = simulation.GetAt(cShapePos.x + pos.x + x, cShapePos.y + pos.y + y);
                if (particle != nullptr && particle->occupied)
                    return true;
            }
        }
    }

    return false;
}

bool Game::IsShapeInvalid() {
    Rectangle rect = GetShapeRect(currentShape);

    if (cShapePos.x + rect.x * tileSize < 0 || cShapePos.x + (rect.x + rect.width) * tileSize > simulation.width)
        return true;
    if (cShapePos.y + rect.y * tileSize < 0 || cShapePos.y + (rect.y + rect.height) * tileSize > simulation.height)
        return true;

    return false;
}

ShapeData Game::NewShape() {
    return ShapeData {
        type: GetRandomValue(0, totalShapes - 1),
        color: GetRandomValue(0, totalColors - 1),
        style: GetRandomValue(0, totalStyles - 1),
        rotation: 0
    };
}

Rectangle GetShapeRect(ShapeData shape) {
    int size = shapeTypes[shape.type].size;
    const auto bitmap = shapeTypes[shape.type].rotations[shape.rotation].bitmap;
    Rectangle rect = {(float) size, (float) size, 0, 0};

    for (int i = 0; i < (signed) bitmap.size(); i++) {
        if (!bitmap[i]) continue;
        
        Vector2 pos = IndexToPos(i, size);

        if (pos.x < rect.x)
            rect.x = pos.x;
        if (pos.y < rect.y)
            rect.y = pos.y;
        if (pos.x + 1 > rect.width)
            rect.width = pos.x + 1;
        if (pos.y + 1 > rect.height)
            rect.height = pos.y + 1;
    }

    return rect;
}

void DrawShape(ShapeData shape, Vector2 pos, float scale) {
    Texture2D &blocks = GetTexture(Textures::blocks);
    int size = shapeTypes[shape.type].size;
    const auto bitmap = shapeTypes[shape.type].rotations[shape.rotation].bitmap;

    Rectangle src = {
        (float) shape.style * tileSize,
        (float) shape.color * tileSize,
        tileSize,
        tileSize
    };

    for (int i = 0; i < (signed) bitmap.size(); i++) {
        if (!bitmap[i]) continue;

        Rectangle dest = {
            pos.x + (i % size) * tileSize * scale,
            pos.y + (float) std::floor(i / size) * tileSize * scale,
            tileSize * scale,
            tileSize * scale
        };

        DrawTexturePro(blocks, src, dest, {0, 0}, 0, WHITE);
    }
}
