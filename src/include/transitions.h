#pragma once
#include "common.h"
#include "easing.h"

class Transition {
public:
    virtual void Draw() = 0;
    virtual bool isFinished() = 0;
};

class BlockFadeTransition : public Transition {
public:
    BlockFadeTransition(Color color, float duration, int tileSize, EaseFunction easingFunction=&EaseLinearNone) 
        : color(color), duration(duration), tileSize(tileSize), timer(0), easingFunction(easingFunction) {};

    void Draw() override;
    bool isFinished() override;

private:
    Color color;
    float duration;
    int tileSize;
    int timer;
    EaseFunction easingFunction;
};
