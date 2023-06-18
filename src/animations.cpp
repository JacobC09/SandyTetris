#include "game.h"
#include "animations.h"
#include "debug.h"

void ConnectionAnim::update(Game* game) {
    const int waitBeforeFade = 10;
    const int maxFallBackDistance = 16;

    timer++;
    bool isWhite = timer % 40 < 20;
    int x = (timer - waitBeforeFade) * 2;

    for (int i = positions.size() - 1; i > -1; i--) {
        Position pos = positions[i];

        if (pos.x < x) {
            if (GetRandomValue(0, std::max(maxFallBackDistance - (x - pos.x), 0)) == 0) {
                positions.erase(positions.begin() + i);
                game->simulation.SetAt(pos.x, pos.y, SandParticle{false});
            }
        }

        if (isWhite)
            DrawPixel(pos.x, pos.y, WHITE);
    }

    if (x > game->simulation.width + maxFallBackDistance) {
        active = false;
        positions.clear();
        finished = true;
    }
}

void FallingParticleAnim::update() {
    timer++;

    for (auto &particle : particles) {
        // Gravity
        particle.vel.y = std::min(particle.vel.y + gravity, maxFallSpeed);

        // Horizontal Friction
        if (std::abs(particle.vel.x) < horizontalDrag) {
            particle.vel.x = 0;
        } else {
            particle.vel.x += particle.vel.x < 0 ? horizontalDrag : -horizontalDrag; 
        }

        // Velocity
        particle.pos.x += particle.vel.x;
        particle.pos.y += particle.vel.y;

        if (collision) {
            // If hit top
            if (particle.pos.y < boundingBox.y) {
                particle.pos.y -= particle.pos.y;
                particle.vel.y = -particle.vel.y * 0.9f;
            }
            
            // If it bottom
            if (particle.pos.y >= boundingBox.height) {
                particle.pos.y -= particle.pos.y - boundingBox.height;
                particle.vel.y = -particle.vel.y * GetRandomValue(65, 85) / 100.0f;
            }

            // If hit right
            if (particle.pos.x < boundingBox.x) {
                particle.pos.x -= particle.pos.x;
                particle.vel.x = -particle.vel.x * 0.9f;
            }

            // If hit left
            if (particle.pos.x >= boundingBox.width) {
                particle.pos.x -= particle.pos.x - boundingBox.width;
                particle.vel.x = -particle.vel.x * 0.9f;
            }
        }

        if (particle.size == 1) {
            DrawPixel(particle.pos.x, particle.pos.y, particle.color);
        } else {
            DrawRectangleRec({particle.pos.x, particle.pos.y, (float) particle.size, (float) particle.size}, particle.color);
        }
    }

    // Remove a random amount of particles if there are too many
    if (particles.size() > 2000) {
        int removeAmount = GetRandomValue(10, 30);

        while (--removeAmount) {
            particles.erase(particles.begin() + GetRandomValue(0, particles.size() - 1));
        }
    }
}

void GameOverAnim::update(Game* game) {
    timer++;
    
    int y = game->simulation.height - timer * 2;

    if (y < 0) {
        finished = true;
        active = false;
        return;
    }

    for (int yOffset = 1; yOffset > -1; yOffset--) {
        if (y + yOffset < 0) break;

        for (int x = 0; x < game->simulation.width; x++) {
            SandParticle* particle = game->simulation.GetAt(x, y);
            if (particle == nullptr || !particle->occupied) continue;

            if (GetRandomValue(0, 3) != 0)
                game->simulation.SetAt(x, y + yOffset, SandParticle{false});
        }
    }
}

void LevelUpAnimation::update(Game* game) {
    const int opacityDuration = 50;
    const int fadeDelay = 5;
    const int maxFallBackDistance = 16;

    timer++;
    tint = Color {0, 0, 0, (unsigned char) EaseCubicInOut(std::min(timer, opacityDuration), opacityDuration, 0, 150)};
    int x = (timer - opacityDuration - fadeDelay) * 2;

    bool isWhite = timer % 40 < 20;

    for (int i = positions.size() - 1; i > -1; i--) {
        Position pos = positions[i];
        SandParticle* particle = game->simulation.GetAt(pos.x, pos.y);
        if (!particle->occupied) continue;

        DrawPixel(pos.x, pos.y, isWhite ? WHITE : particle->color);
        
        // Randomly Remove Pixel
        if (pos.x < x) {
            if (GetRandomValue(0, std::max(maxFallBackDistance - (x - pos.x), 0)) == 0) {
                positions.erase(positions.begin() + i);
                game->simulation.SetAt(pos.x, pos.y, SandParticle{false});
            }
        }
    }

    if (x > game->simulation.width + maxFallBackDistance) {
        positions.clear();
        finished = true;
    }
}

void FadeInCenterText::update() {
    // Increment the timer vairable
    timer++;

    // Calculate the animation alpha value
    float alpha;
    if (stayTime < 0 && timer > fadeInTime) {
        alpha = 1;
    } else {
        alpha = std::min((float) timer/fadeInTime, 1.0f) - (float) std::max(timer - fadeInTime - stayTime, 0) / fadeOutTime;
    }

    // Calculate the width of th text
    int width = 0;  
    for (auto &str : texts) {
        width += font->Measure(str) * size;
    }

    // Calculate the render position of the text
    Vector2 pos = {
        (screenWidth - width) / 2.0f,
        screenHeight / 2 - font->height * size
    };

    // Update the alpha channel of the colors
    std::vector<Color> alphaColors;
    for (auto& color : colors) {
        alphaColors.push_back(ColorAlpha(color, alpha));
    }

    // Render the text
    font->RenderColored(texts, pos, size, alphaColors);

    // End the animation
    if (timer > fadeInTime + stayTime + fadeOutTime && stayTime >= 0) {
        finished = true;
    }
}
