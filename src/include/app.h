#pragma once
#include "common.h"
#include "transitions.h"
#include "sand.h"

// Forward Declaration
class Application;

Vector2 IndexToPos(int index, int sideLength);

struct Timer {
    int total;
    int timer = 0;

    bool done() {return timer >= total;}
    void update() {if (++timer > total) timer = 0;}
    void reset() {timer = 0;}
};

// UI Constants
const int screenWidth = 880;
const int screenHeight = 640;
const int tileSize = 8;
const int boardWidth = 10;
const int boardHeight = 16;
const float scale = 4;

// Colors
const Color darkBrown = Color {139, 28, 3, 255};

// Game
struct ShapeData {
    int type;
    int color;
    int style;
    int rotation;
};

Rectangle GetShapeRect(ShapeData shape);
void DrawShape(ShapeData shape, Vector2 pos, float scale);

class Game {
public:
    Game() = default;
    Game(Application* app) : app(app) {};

    void Load();
    void NewGame();
    void Update();

    void DrawBg();
    void DrawSandToTex();
    void MoveShape();
    void TryToCorrectShape();
    void RotateShape();
    void SpawnShape();
    void CheckShapeCollision(Vector2 mouvement);
    void TurnShapeToSand();

    bool IsShapeColliding();
    bool IsShapeInvalid();
    ShapeData NewShape();

private:
    int sinceSandUpdate;
    int bgAnimationTimer;
    bool gameOver = false;
    Timer bgAnimation;
    Simulation simulation;
    RenderTexture2D boardTex;
    Image blocksImg;
    Rectangle boardRect;
    Application* app;
    
    // Current Shape
    ShapeData currentShape;
    Vector2 cShapePos;

    // Stats
    float shapeMoveSpeed;
    float shapeFallSpeed;
};

// App
class Application {
public:
    enum States {
        Running
    };

    struct Settings {
        bool music = false;
    };

    int time;
    int iResoluationLoc;
    int iTimeLoc;
    States state;
    Game game;
    Settings settings;
    std::vector<Transition*> transitions;
    
    Application() = default;
    
    void Load();
    void Run();
    void Unload();
};

struct SnowParticle {
    Vector2 pos;
    int size;
    int speed;
    int lifetime;
};
