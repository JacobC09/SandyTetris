#include "intro.h"
#include "assets.h"
#include "debug.h"

const std::string text = "Made by Jake";
const std::vector<std::string> coloredText = {"Made by ", "Jake"};

void Intro::Load() {
    timer = 0;
    renderTexture = LoadRenderTexture(app->font.Measure(text), app->font.height);
    particleAnim.reset();
}

void Intro::Update() {
    const int introTextDuration = 100;
    const int afterParticleTimerDuration = 100;
    const int textFadeInDelay = 40;
    const float textSize = 2;

    timer++;
    ClearBackground({12, 5, 1, 255});

    if (!particleAnim.active) {
        if (timer > textFadeInDelay) {
            BeginTextureMode(renderTexture);
            app->font.RenderColored(coloredText, {0, 0}, 1, {WHITE, Colors::orange3});
            EndTextureMode();

            Rectangle source {
                0, 0,
                (float) renderTexture.texture.width,
                (float) -renderTexture.texture.height
            };

            Rectangle dest = {
                (float) (screenWidth - renderTexture.texture.width * textSize) / 2,
                (float) screenHeight / 2 - renderTexture.texture.height * textSize,
                (float) renderTexture.texture.width * textSize,
                (float) renderTexture.texture.height * textSize
            };

            float alpha = std::min((float) (timer - textFadeInDelay) / 90, 1.0f);
            DrawTexturePro(renderTexture.texture, source, dest, {0, 0}, 0, ColorAlpha(WHITE, alpha));
            

            if (timer == textFadeInDelay + introTextDuration) {
                SpawnParticles(dest, textSize);
                afterParticleTimer = 0;
            }
        }
    } else {
        particleAnim.update();

        if (++afterParticleTimer > afterParticleTimerDuration) {
            UpdateTransitions();
        }
    }
}

void Intro::SpawnParticles(Rectangle destOffset, float scale) {
    Image image = LoadImageFromTexture(renderTexture.texture);
    
    particleAnim = FallingParticleAnim {
        .particles = {},
        .boundingBox = {},
        .collision = false,

        .gravity = 0.15,
        .maxFallSpeed = 20,
        .horizontalDrag = 0.001,
    };

    for (int x = 0; x < renderTexture.texture.width; x++) {
        for (int y = 0; y < renderTexture.texture.height; y++) {
            Color color = GetImageColor(image, x, renderTexture.texture.height - y - 1);

            if (color.a > 0) {
                particleAnim.particles.push_back(FallingParticle {
                    .pos = {destOffset.x + x * scale, destOffset.y + y * scale},
                    .vel = {GetRandomValue(-200, 200) / 100.0f, GetRandomValue(-200, 400) / 100.0f},
                    .color = color,
                    .size = (int) scale
                });
            }
        }
    }

    particleAnim.start();
}

void Intro::UpdateTransitions() {
    Transition* first = app->GetTransition("intro-arrow");

    if (first == nullptr) {
        app->AddTransition<ArrowTransition>("intro-arrow", Colors::orange0, 40, 260);
    } else if (first->isFinished()) {
        app->AddTransition<ReverseArrowTransition>("intro-arrow-reversed", Colors::orange0, 40, 260);
        app->state = Application::States::Game;
        // Add game start delay and change the other one to look like this and add count down and level diap[ly gaof rgame as well as a level system that adds new color and add green color and other colors if you want ikd but just do whatever the fk u want]
    }
}
