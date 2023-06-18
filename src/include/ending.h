#pragma once
#include "app.h"
#include "animations.h"

class Ending : public Screen {
public:
    Ending(Application* app) : app(app) {};

    void Load() override;
    void Update() override;

private:
    Application* app;
    int inbetweenTimer;
    std::vector<FadeInCenterText> texts;
};