#include "transitions.h"
#include "easing.h"

void ArrowTransition::Draw()  {
    if (timer < duration)
        timer++;

    float x = easingFunction(timer, duration, -arrowOffset, GetScreenWidth() + arrowOffset);

    if (x > 0) 
        DrawRectangleRec({0, 0, x, (float) GetScreenHeight()}, color);

    DrawTriangle({x, (float) GetScreenHeight()}, {x + arrowOffset, GetScreenHeight() * 0.5f}, {x, 0}, color);
}

bool ArrowTransition::isFinished() {
    return timer >= duration;
}

void ReverseArrowTransition::Draw()  {
    if (timer < duration)
        timer++;

    float x = easingFunction(timer, duration, 0, GetScreenWidth() + arrowOffset);

    DrawRectangleRec({x, 0, GetScreenWidth() - x, (float) GetScreenHeight()}, color);
    DrawTriangle({x - arrowOffset, 0}, {x, GetScreenWidth() * 0.5f}, {x, 0}, color);
    DrawTriangle({x, GetScreenWidth() * 0.5f}, {x - arrowOffset, (float) GetScreenHeight()}, {x, (float) GetScreenHeight()}, color);
}

bool ReverseArrowTransition::isFinished() {
    return timer >= duration;
}
