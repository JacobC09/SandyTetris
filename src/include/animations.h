#pragma once
#include "common.h"

class Game;

struct Animation {
    int timer = 0;
    bool active = false;
    bool finished = false;

    void reset() {timer = 0; finished = false; active = false;}
    void start() {timer = 0; finished = false; active = true;}
    bool isFinished() {return finished;}
};

struct ConnectionAnim : Animation {
    std::vector<Position> positions;
    int highestPoint;

    void update(Game* game);
};

struct FallingParticle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    int size = 1;
};

struct FallingParticleAnim : Animation {
    std::vector<FallingParticle> particles;
    Rectangle boundingBox;
    bool collision = false;

    float gravity = 0.05;
    float maxFallSpeed = 3;
    float horizontalDrag = 0.0005;

    void update();
};

struct GameOverAnim : Animation {
    void update(Game* game);
};
