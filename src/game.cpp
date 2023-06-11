#include <cmath>
#include "game.h"
#include "assets.h"
#include "app.h"
#include "debug.h"

void Game::Load() {
    simulation = Simulation(boardWidth * tileSize, boardHeight * tileSize);
    boardTex = LoadRenderTexture(simulation.width, simulation.height);
    bgAnimation = Timer {240};
    blocksImg = LoadImageFromTexture(GetTexture(Textures::blocks));
}

void Game::NewGame() {
    simulation.Reset();
    bgAnimation.reset();
    sinceSandUpdate = 0;
    shapeMoveSpeed = 2;
    shapeFallSpeed = 1;
    gameOver = false;
    connectionAnim = ConnectionAnim {.positions = {}};
    textParticles.clear();

    boardRect = {
        0, 0, 
        boardWidth * tileSize * scale, 
        boardHeight * tileSize * scale
    };

    nextShapeRect = {
        0, 
        0,
        tileSize * scale * 5 + panelMarginSide * 2,
        tileSize * scale * 3 + panelMarginTop * 3,
    };

    infoPanelRect = {
        0, 0,
        nextShapeRect.width,
        panelMarginTop * 3 + (app->font.height) * 6
    };

    boardRect.x = (screenWidth - boardRect.width - nextShapeRect.width / 2) / 2;
    boardRect.y = (screenHeight - boardRect.height) / 2;
    nextShapeRect.x = boardRect.x + boardRect.width + panelPadding;
    nextShapeRect.y = (screenHeight - (nextShapeRect.height + infoPanelRect.height + panelPadding)) / 2;
    infoPanelRect.x = nextShapeRect.x;
    infoPanelRect.y = nextShapeRect.y + nextShapeRect.height + panelPadding;

    stats = Statistics {
        .score = 0,
        .lines = 0
    };

    level = 1;

    nextShape = NewShape();
    SpawnShape();
}

void Game::Update() {
    if (gameOver) {
        Transition* transition = app->GetTransition("game-over-arrow");

        if (transition == nullptr) {
            app->AddTransition<ArrowTransition>("game-over-arrow", Colors::orange0, 40, 260);
        } else if (transition->isFinished()) {
            NewGame();
        }
    }

    if (!connectionAnim.active && !gameOver) {
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
    DrawNextShape();
    DrawInfoPanel();

    BeginTextureMode(boardTex);
        ClearBackground(Colors::dim);
        DrawSandToTex();

        if (currentShape.type != -1)
            DrawShape(currentShape, {std::floor(cShapePos.x), std::floor(cShapePos.y)}, 1);
        
        if (connectionAnim.active) {
            UpdateConnectAnim();
        }

    EndTextureMode();
    DrawTexturePro(boardTex.texture, {0, 0, (float) boardTex.texture.width, (float) -boardTex.texture.height}, boardRect, {0, 0}, 0, WHITE);

    UpdateTextParticles();
}

void Game::DrawBg() {
    // Draw Background
    BeginShaderMode(GetShader(Shaders::Heat));
        Texture2D &texture = (bgAnimation.timer < bgAnimation.total / 2) ? GetTexture(Textures::desertBg2) : GetTexture(Textures::desertBg1);
        DrawTexturePro(texture, {0, 0, (float) texture.width, (float) texture.height}, {0, 0, (float) screenWidth, (float) screenHeight}, {0, 0}, 0, WHITE);
    EndShaderMode();

    bgAnimation.update();

    // Draw Board Border
    DrawBorder(boardRect, panelBorderThickness, Colors::orange0);
}

void Game::DrawNextShape() {
    DrawRectangleRec(nextShapeRect, Colors::dim);
    DrawBorder(nextShapeRect, panelBorderThickness, Colors::orange0);
    
    float y = nextShapeRect.y + panelMarginTop;

    // Next Shape Text
    const float nextShapeTextSize = 2;
    Vector2 nextShapeTextPos = {
        nextShapeRect.x + nextShapeRect.width / 2,
        y
    };

    y += app->font.height * nextShapeTextSize;
    app->font.RenderCentered("Next Shape", nextShapeTextPos, nextShapeTextSize, Colors::orange1, true, false);

    // Shape Img
    const int shapeScale = 3;
    Rectangle shapeRect = GetShapeRect(nextShape);
    Vector2 center = {
        nextShapeRect.x + nextShapeRect.width / 2,
        y + (nextShapeRect.height - (y - nextShapeRect.y)) / 2
    };
    Vector2 shapePos = {
        center.x - (shapeRect.x + shapeRect.width / 2) * shapeScale * tileSize,
        center.y - (shapeRect.y + shapeRect.height / 2) * shapeScale * tileSize,
    };

    DrawShape(nextShape, shapePos, shapeScale);
}

void Game::DrawInfoPanel() {
    DrawRectangleRec(infoPanelRect, Colors::dim);
    DrawBorder(infoPanelRect, panelBorderThickness, Colors::orange0);
    
    float y = infoPanelRect.y + panelMarginTop;
    
    // Score
    const float textSize = 2;
    app->font.Render("Score: ", {infoPanelRect.x + panelMarginSide, y}, textSize, Colors::orange1);
    
    // Score Value
    std::string scoreStr = std::to_string(stats.score);
    Vector2 scoreTextPos = {
        infoPanelRect.x + infoPanelRect.width - panelMarginSide - app->font.Measure(scoreStr) * textSize, 
        y
    };
    app->font.Render(scoreStr, scoreTextPos, textSize, Colors::orange2);\
    y += app->font.height * 2 + panelMarginTop / 2; 

    // Lines
    app->font.Render("Lines: ", {infoPanelRect.x + panelMarginSide, y}, textSize, Colors::orange1);
        
    // Lines Value
    std::string linesStr = std::to_string(stats.lines) + "/32";
    Vector2 linesTextPos = {
        infoPanelRect.x + infoPanelRect.width - panelMarginSide - app->font.Measure(linesStr) * textSize, 
        y
    };
    app->font.Render(linesStr, linesTextPos, textSize, Colors::orange2);
    y += app->font.height * 2 + panelMarginTop / 2;

    // Level
    app->font.Render("Level: ", {infoPanelRect.x + panelMarginSide, y}, textSize, Colors::orange1);
    
    // Level Value
    std::string levelStr = std::to_string(level);
    Vector2 levelTextPos = {
        infoPanelRect.x + infoPanelRect.width - panelMarginSide - app->font.Measure(levelStr) * textSize, 
        y
    };
    app->font.Render(levelStr, levelTextPos, textSize, Colors::orange2);
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
    currentShape = nextShape;
    nextShape = NewShape();
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
            connectionAnim.highestPoint = simulation.height;
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
        
        if (pos.y < connectionAnim.highestPoint)
            connectionAnim.highestPoint = pos.y;

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
        stats.lines++;

        int yStart = std::max(boardRect.y + connectionAnim.highestPoint * scale - 10, boardRect.y + 150);

        // Spawn Text Particles
        textParticles.push_back(TextParticle {
            .text = "Congrats!",
            .startPos = {boardRect.x + boardRect.width / 2, (float) yStart},
            .size = 2,
            .color = Colors::orange2,
            .offset = 0
        });
        textParticles.push_back(TextParticle {
            .text = "+100",
            .startPos = {boardRect.x + boardRect.width / 2, yStart + app->font.height * 2 + 6},
            .size = 1.8,
            .color = Colors::orange3,
            .offset = 5
        });

        stats.score += 100;
    }
}

void Game::UpdateTextParticles() {
    for (int i = textParticles.size() - 1; i > -1; i--) {
        TextParticle &textParticle = textParticles[i];
        textParticle.timer++;

        int fadeInTime = 35 + textParticle.offset;
        int fadeOutTime = std::max(15 - textParticle.offset, 0);

        int fadeInTimer = std::min(textParticle.timer, fadeInTime);
        int fadeOutTimer = std::max(textParticle.timer - fadeInTime, 0);

        float fadeInOpacity = std::min(EaseCubicOut(fadeInTimer, fadeInTime, 0, 1), 1.0f);
        float fadeOutOpacity = EaseCubicIn(fadeOutTimer, fadeOutTime, 0, 0.8f);
        Color color = ColorAlpha(textParticle.color, fadeInOpacity - fadeOutOpacity);

        Vector2 pos = {
            textParticle.startPos.x,
            textParticle.startPos.y - EaseCubicOut(fadeInTimer, fadeInTime, 0, 100) + EaseCubicIn(fadeOutTimer, fadeOutTime, 0, 10)
        };

        app->font.RenderCentered(textParticle.text, pos, textParticle.size, color, true, false);
    
        if (textParticle.timer > fadeInTime + fadeOutTime) {
            textParticles.erase(textParticles.begin() + i);
        }
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
        .type = GetRandomValue(0, totalShapes - 1),
        .color = GetRandomValue(0, totalColors - 1),
        .style = GetRandomValue(0, totalStyles - 1),
        .rotation = 0
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

    return {rect.x, rect.y, rect.width - rect.x, rect.height - rect.y};
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

void DrawBorder(Rectangle rect, int thickness, Color color) {
    DrawRectangleLinesEx({
        rect.x - thickness, 
        rect.y - thickness,  rect.width + thickness * 2, 
        rect.height + thickness * 2
    }, thickness, color);
}
