#include "transitions.h"
#include "easing.h"

void BlockFadeTransition::Draw()  {
    const int lookAheadFactor = 4;

    double interpolatedTimer = easingFunction(timer, duration, 0, 1) + (float) 1/lookAheadFactor;

    int currentX = GetScreenWidth() * interpolatedTimer;
    int currentY = GetScreenHeight() * interpolatedTimer;

    for (int x = 0; x < GetScreenWidth(); x += tileSize) {
        for (int y = 0; y < GetScreenHeight(); y += tileSize) {
            int flippedX = GetScreenWidth() - x;

            if (!(flippedX < currentX && y < currentY)) continue;
            
            // Flip x-axis so the origin is topright
            int size = std::min(currentX - flippedX, currentY - y) / lookAheadFactor;
            
            if (size < tileSize)
                DrawRectangle(x + (tileSize - size) / 2, y + (tileSize - size) / 2, size, size, color);
            else
                DrawRectangle(x, y, tileSize, tileSize, color);
        }
    }

    timer++;
}

bool BlockFadeTransition::isFinished() {
    return timer >= duration;
}
