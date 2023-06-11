#include "transitions.h"
#include "easing.h"

void ArrowTransition::Draw()  {
    if (timer < duration)
        timer++;

    float x = easingFunction(timer, duration, -arrowOffset, GetScreenWidth() + arrowOffset);
    DrawRectangleRec({0, 0, x < 0 ? 0 : x, (float) GetScreenHeight()}, color);
    DrawTriangle({x, (float) GetScreenHeight()}, {x + arrowOffset, GetScreenHeight() * 0.5f}, {x, 0}, color);
}

bool ArrowTransition::isFinished() {
    return timer >= duration;
}
