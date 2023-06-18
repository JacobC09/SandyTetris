#include <cmath>
#include "app.h"
#include "game.h"
#include "assets.h"
#include "debug.h"

void Game::Load() {
    simulation = Simulation(boardWidth * tileSize, boardHeight * tileSize);
    boardTex = LoadRenderTexture(simulation.width, simulation.height);
    bgAnimation = Timer {240};
    blocksImg = LoadImageFromTexture(GetTexture(Textures::blocks));
    levelIndex = 0;
}

void Game::NewGame() {
    simulation.Clear();
    bgAnimation.reset();
    levelUpAnim.reset();
    connectionAnim.reset();
    gameOverAnim.reset();
    gameOverParticleAnim.reset();
    textParticles.clear();
    sinceSandUpdate = 0;
    startDelay = 90;
    comboTimer = 0;
    comboCount = 0;
    verticalShakeTimer = 0;
    horizontalShakeTimer = 0;
    
    // Game Over
    gameOverTimer = 0;
    gameOver = false;

    // Panel Positions
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

    // Stats
    level = levels[levelIndex];
    stats = Statistics {
        .score = 0,
        .clears = 0
    };

    localScore = stats.score;

    // Shape
    nextShape = GenShape();
    SpawnShape();
    nextShape = currentShape;
}

void Game::Update() {
    bool paused = false;
    Vector2 screenShake = {0, 0};

    if (gameOver)
        paused = true;

    if (verticalShakeTimer > 0) {
        verticalShakeTimer--;
        screenShake.y = GetRandomValue(-scale, scale);
        boardRect.y -= screenShake.y;
        nextShapeRect.y -= screenShake.y;
        infoPanelRect.y -= screenShake.y;
    }

    if (horizontalShakeTimer > 0) {
        horizontalShakeTimer--;
        screenShake.x = GetRandomValue(-scale, scale);
        boardRect.x -= screenShake.x;
        nextShapeRect.x -= screenShake.x;
        infoPanelRect.x -= screenShake.x;
    }
    
    if (startDelay) {
        startDelay--;
        paused = true;

        if (startDelay == 50) {
            textParticles.push_back(TextParticle {
                .text = "Level " + std::to_string(levelIndex + 1),
                .startPos = {boardRect.x + boardRect.width / 2, boardRect.y + boardRect.height / 2 - 15},
                .size = 3,
                .color = Colors::orange2,
                .fadeInTime = 30,
                .fadeOutTime = 10,
                .ascendDistance = 40,
                .descendDistance = 5,
                .startDelay = 0,
            });
        }
        
        if (startDelay == 0)
            nextShape = GenShape();
    }


    if (!connectionAnim.active && !levelUpAnim.active && !paused) {
        if (++sinceSandUpdate > 1) {
            simulation.Step();
            sinceSandUpdate = 0;
        }

        if (currentShape.type == -1)
            SpawnShape();

        MoveShape();

        if (currentShape.type != -1) {
            RotateShape();
            FindConnectedSand();
        }

        if (comboTimer) {
            comboTimer--;
        } else {
            comboCount = 0;
        }
    }

    DrawBg();
    DrawNextShape();
    DrawInfoPanel();

    BeginTextureMode(boardTex);
        ClearBackground(Colors::dim);

        DrawSandToTex();

        if (currentShape.type != -1 && !levelUpAnim.active && !paused)
            DrawShape(currentShape, {std::floor(cShapePos.x), std::floor(cShapePos.y)}, 1);
        
        if (connectionAnim.active) {
            UpdateConnectAnim();
        }

        if (gameOver) {
            UpdateGameOverAnim();
        }

        if (levelUpAnim.active) {
            UpdateLevelUpAnim();
        }

    EndTextureMode();

    Rectangle boardTexSource = {0, 0, (float) boardTex.texture.width, (float) -boardTex.texture.height};
    DrawTexturePro(boardTex.texture, boardTexSource, boardRect, {0, 0}, 0, WHITE);

    UpdateTextParticles();
    
    // Add back the shake offset
    boardRect.x += screenShake.x;
    boardRect.y += screenShake.y;
    nextShapeRect.x += screenShake.x;
    nextShapeRect.y += screenShake.y;
    infoPanelRect.x += screenShake.x;
    infoPanelRect.y += screenShake.y;
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
    std::string scoreStr = std::to_string(localScore);
    Vector2 scoreTextPos = {
        infoPanelRect.x + infoPanelRect.width - panelMarginSide - app->font.Measure(scoreStr) * textSize, 
        y
    };

    if (localScore < stats.score) {
        localScore += scoreIncrement;
        if (std::abs(localScore - stats.score) < scoreIncrement) {
            localScore = stats.score;
        }
        scoreTextPos.x -= GetRandomValue(-textSize, textSize);
        scoreTextPos.y -= GetRandomValue(-textSize, textSize);
    }

    app->font.Render(scoreStr, scoreTextPos, textSize, Colors::orange2);\
    y += app->font.height * 2 + panelMarginTop / 2; 

    // Lines
    app->font.Render("Lines: ", {infoPanelRect.x + panelMarginSide, y}, textSize, Colors::orange1);
        
    // Lines Value
    std::string linesStr = std::to_string(stats.clears) + "/" + std::to_string(level.requiredClears);
    Vector2 linesTextPos = {
        infoPanelRect.x + infoPanelRect.width - panelMarginSide - app->font.Measure(linesStr) * textSize, 
        y
    };
    app->font.Render(linesStr, linesTextPos, textSize, Colors::orange2);
    y += app->font.height * 2 + panelMarginTop / 2;

    // Level
    app->font.Render("Level: ", {infoPanelRect.x + panelMarginSide, y}, textSize, Colors::orange1);
    
    // Level Value
    std::string levelStr = std::to_string(levelIndex + 1);
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

            if (levelUpAnim.active) {
                DrawPixel(x, y, ColorAlphaBlend(particle->color, levelUpAnim.tint, WHITE));
            } else {
                DrawPixel(x, y, particle->color);
            }
        }
    }
}

void Game::MoveShape() {
    Vector2 movement = {0, 0};

    bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);

    movement.y += down ? level.fallSpeed * 2 : level.fallSpeed;

    bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);

    if (right && !left) {
        movement.x += level.horizontalSpeed;
    } else if (left && !right) {
        movement.x -= level.horizontalSpeed;
    }

    CheckShapeCollision(movement);
}

void Game::RotateShape() {
    bool up = IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
    bool down = IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN);

    int totalRotations = (signed) shapeTypes[currentShape.type].rotations.size();
    int oldRotation = currentShape.rotation;

    bool rotated = true;

    if (up && !down) {
        if (++currentShape.rotation >= totalRotations)
            currentShape.rotation = 0;
        
    } else if (up && !down) {
        if (--currentShape.rotation < 0)
            currentShape.rotation = totalRotations - 1;
    } else {
        rotated = false;
    }

    if (currentShape.rotation != oldRotation && !IsKeyDown(KEY_SPACE)) {
        if (IsShapeColliding()) {
            currentShape.rotation = oldRotation;
            rotated = false;
        } else {
            TryToCorrectShape();
        }
    }

    if (rotated) {
        PlaySound(GetSound(Sounds::block_rotate));
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
    nextShape = GenShape();
    Rectangle shapeRect = GetShapeRect(currentShape);
    
    cShapePos = Vector2 {
        simulation.width / 2 - (shapeRect.x + shapeRect.width / 2) * tileSize,
        -shapeRect.y * tileSize
    };

    if (IsShapeColliding()) {
        TurnShapeToSand();
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
        PlaySound(GetSound(Sounds::block_fall));
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
                Position particlePos = {(int) std::floor(cShapePos.x + pos.x + x), (int) std::floor(cShapePos.y + pos.y + y)};

                if (!simulation.ValidPosition(particlePos.x, particlePos.y)) continue;

                simulation.SetAt(particlePos.x, particlePos.y, SandParticle {
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
            if (stats.clears + 1 == level.requiredClears) {
                levelUpAnim.start();
                levelUpAnim.positions = visited;
                stats.clears++;
                CalculateScore();
                SpawnBoardText("Level Up!", Colors::orange2, "Yay", Colors::orange4);
                PlaySound(GetSound(Sounds::level_up));
            } else {
                connectionAnim.start();
                connectionAnim.positions = visited;

                const int totalComboSounds = 6;
                const Sounds comboSounds[totalComboSounds] = {
                    Sounds::clear_1,
                    Sounds::clear_2,
                    Sounds::clear_3,
                    Sounds::clear_4,
                    Sounds::clear_5,
                    Sounds::clear_6,
                };

                PlaySound(GetSound(comboSounds[std::min(comboCount, totalComboSounds - 1)]));
            }
            verticalShakeTimer = 10;
            horizontalShakeTimer = 10;
        }
    }
}

void Game::UpdateConnectAnim() {
    connectionAnim.update(this);

    if (connectionAnim.isFinished()) {
        int startScore = stats.score;
        CalculateScore();

        stats.clears++;
        comboTimer = 120;
        comboCount++;

        const int maxComboNames = 6;
        const std::string comboNames[maxComboNames] {
            "Congrats!",
            "Double Combo",
            "Triple Combo",
            "Quadrouple Combo",
            "Omega Holy Combo",
            "Ultra Omega Jesus Combo"
        };

        std::string largeString = comboNames[std::min(comboCount, maxComboNames) - 1];
        Color largeColor = comboCount == 1 ? Colors::orange2 : Colors::orange4;
        std::string smallString = "+" + std::to_string(stats.score - startScore);
        Color smallColor = Colors::orange3;

        SpawnBoardText(largeString, largeColor, smallString, smallColor);
    }
}

void Game::UpdateGameOverAnim() {
    const int totalDuration = 240;
    const int startDelay = 20;

    gameOverTimer++;

    if (gameOverTimer < startDelay)
        return;

    if (!gameOverAnim.finished && gameOverTimer < totalDuration) {
        if (!gameOverAnim.active) {
            gameOverAnim.start();
            PlaySound(GetSound(Sounds::game_over_1));
        }

        gameOverAnim.update(this);
        return;
    }

    if (gameOverParticleAnim.active) {
        gameOverParticleAnim.update();
    } else if (!gameOverParticleAnim.finished) {
        gameOverParticleAnim = FallingParticleAnim {
            .particles = {},
            .boundingBox = Rectangle {0, 0, (float) simulation.width, (float) simulation.height},
            .collision = true,
        };

        gameOverParticleAnim.start();
        PlaySound(GetSound(Sounds::game_over_2));


        // Add the existing particles to the animation
        for (int simY = 0; simY < simulation.height; simY++) {
            for (int simX = 0; simX < simulation.width; simX++) {
                SandParticle* particle = simulation.GetAt(simX, simY);

                if (particle != nullptr && particle->occupied) {
                    Vector2 vel = {GetRandomValue(-3, 3) / 10.0f, 0};

                    gameOverParticleAnim.particles.push_back(FallingParticle {
                        .pos = {(float) simX, (float) simY},
                        .vel = vel,
                        .color = particle->color
                    });
                }
            }
        }

        simulation.Clear();
    }

    if (gameOverTimer > totalDuration) {
        Transition* firstTransition = app->GetTransition("game-over-arrow");

        if (firstTransition == nullptr) {
            app->AddTransition<ArrowTransition>("game-over-arrow", Colors::orange0, 40, 260);
        } else if (firstTransition->isFinished()) {
            app->AddTransition<ReverseArrowTransition>("game-over-arrow-reversed", Colors::orange0, 40, 260);
            NewGame();
        }
    }
}

void Game::UpdateLevelUpAnim() {
    if (!levelUpAnim.finished) {
        levelUpAnim.update(this);
    } else {
        Transition* transition = app->GetTransition("level-up-arrow");

        if (transition == nullptr) {
            app->AddTransition<ArrowTransition>("level-up-arrow", Colors::orange0, 40, 260);
        } else if (transition->isFinished()) {
            app->AddTransition<ReverseArrowTransition>("game-over-arrow", Colors::orange0, 40, 260);
            if (levelIndex < maxLevels - 1) {
                levelIndex++;
                NewGame();
            } else {
                app->state = Application::States::Ending;
            }
        }
    }
}

void Game::UpdateTextParticles() {
    for (int i = textParticles.size() - 1; i > -1; i--) {
        TextParticle &textParticle = textParticles[i];
        textParticle.timer++;

        if (textParticle.timer < textParticle.startDelay)
            continue;

        int fadeInTimer = std::min(textParticle.timer - textParticle.startDelay, textParticle.fadeInTime);
        int fadeOutTimer = std::max(textParticle.timer - textParticle.fadeInTime, 0);

        float fadeInOpacity = std::min(EaseCubicOut(fadeInTimer, textParticle.fadeInTime, 0, 1), 1.0f);
        float fadeOutOpacity = EaseCubicIn(fadeOutTimer, textParticle.fadeOutTime, 0, 0.8f);
        Color color = ColorAlpha(textParticle.color, fadeInOpacity - fadeOutOpacity);

        Vector2 pos = {
            textParticle.startPos.x,
            textParticle.startPos.y - EaseCubicOut(fadeInTimer, textParticle.fadeInTime, 0, textParticle.ascendDistance) 
                + EaseCubicIn(fadeOutTimer, textParticle.fadeOutTime, 0, textParticle.descendDistance)
        };

        app->font.RenderCentered(textParticle.text, pos, textParticle.size, color, true, false);
    
        if (textParticle.timer > textParticle.fadeInTime + textParticle.fadeOutTime) {
            textParticles.erase(textParticles.begin() + i);
        }
    }
}

void Game::SpawnBoardText(std::string largeString, Color largeColor, std::string smallString, Color smallColor) {
    int yStart = std::min(std::max(boardRect.y + simulation.GetHighestPoint() * scale - 16, boardRect.y + 150), boardRect.y + boardRect.height - 16);

    textParticles.push_back(TextParticle {
        .text = largeString,
        .startPos = {boardRect.x + boardRect.width / 2, (float) yStart},
        .size = 2,
        .color = largeColor,
        .fadeInTime = 35,
        .fadeOutTime = 10,
        .ascendDistance = 100,
        .descendDistance = 10,
        .startDelay = 0,
    });
    textParticles.push_back(TextParticle {
        .text = smallString,
        .startPos = {boardRect.x + boardRect.width / 2, yStart + app->font.height * 2 + 6},
        .size = 1.8,
        .color = smallColor,
        .fadeInTime = 38,
        .fadeOutTime = 7,
        .ascendDistance = 100,
        .descendDistance = 10,
        .startDelay = 3,
    });
}

void Game::CalculateScore() {
    stats.score += std::pow(3, comboCount) * 100;
    scoreIncrement = std::max((stats.score - localScore) / 100 * 3, 3);
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

ShapeData Game::GenShape() {
    return ShapeData {
        .type = GetRandomValue(0, totalShapes - 1),
        .color = GetRandomValue(0, level.maxColors - 1),
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

Vector2 IndexToPos(int index, int sideLength) {
    return Vector2 {
        (float) (index % sideLength), 
        (float) std::floor(index / sideLength)
    }; 
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
