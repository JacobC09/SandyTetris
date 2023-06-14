#pragma once
#include "common.h"
#include "easing.h"

class Transition {
public:
    virtual void Draw() = 0;
    virtual bool isFinished() = 0;
};

class ArrowTransition : public Transition {
public:
    ArrowTransition(Color color, float duration, int arrowOffset, EaseFunction easingFunction=EaseCubicInOut) 
        : color(color), duration(duration), arrowOffset(arrowOffset), timer(0), easingFunction(easingFunction) {};

    void Draw() override;
    bool isFinished() override;

private:
    Color color;
    float duration;
    int timer;
    int arrowOffset;
    EaseFunction easingFunction;
};

class ReverseArrowTransition : public Transition {
public:
    ReverseArrowTransition(Color color, float duration, int arrowOffset, EaseFunction easingFunction=EaseCubicInOut) 
        : color(color), duration(duration), arrowOffset(arrowOffset), timer(0), easingFunction(easingFunction) {};

    void Draw() override;
    bool isFinished() override;

private:
    Color color;
    float duration;
    int timer;
    int arrowOffset;
    EaseFunction easingFunction;
};
