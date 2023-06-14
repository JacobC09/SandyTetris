#pragma once
#include "app.h"
#include "animations.h"

class Intro : public Screen {
public:
    Intro(Application* app) : app(app) {};

    void Load() override;
    void Update() override;

private:

    void SpawnParticles(Rectangle destOffset, float scale);
    void UpdateTransitions();

    int timer;
    int afterParticleTimer;
    RenderTexture2D renderTexture;
    FallingParticleAnim particleAnim;
    Application* app;
};