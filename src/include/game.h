
#pragma once
#include "shapes.h"
#include "simulation.h"

class Application;

Vector2 IndexToPos(int index, int sideLength);
void DrawBorder(Rectangle rect, int thickness, Color color);

struct Position {
    int x;
    int y;
};

struct Statistics {
    int score;
    int lines;
};

struct Timer {
    int total;
    int timer = 0;

    bool done() {return timer >= total;}
    void update() {if (++timer > total) timer = 0;}
    void reset() {timer = 0;}
};

struct Animation {
    int timer = 0;
    bool active = false;

    void reset() {timer = 0;}
};

// UI Constants
const int screenWidth = 880;
const int screenHeight = 640;
const int tileSize = 8;
const int boardWidth = 10;
const int boardHeight = 17;
const int panelBorderThickness = 6;
const int panelPadding = 32;
const int panelMarginSide = 16;
const int panelMarginTop = 12;
const float scale = 4;

// Colors
namespace Colors {
    const Color orange0 = Color {139, 28, 3, 255};
    const Color orange1 = Color {181, 64, 25, 255};
    const Color orange2 = Color {225, 110, 51, 255};
    const Color orange3 = Color {231, 128, 54, 255};
    const Color dim = ColorAlpha(BLACK, 0.9);
}

struct ConnectionAnim : Animation {
    std::vector<Position> positions;
    int highestPoint;
};

struct TextParticle : Animation {
    std::string text;
    Vector2 startPos;
    float size;
    Color color;
    int offset;
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
    void DrawNextShape();
    void DrawInfoPanel();
    void DrawSandToTex();
    void MoveShape();
    void TryToCorrectShape();
    void RotateShape();
    void SpawnShape();
    void CheckShapeCollision(Vector2 mouvement);
    void TurnShapeToSand();
    void FindConnectedSand();
    void UpdateConnectAnim();
    void UpdateTextParticles();

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
    Rectangle nextShapeRect;
    Rectangle infoPanelRect;
    Application* app;

    // Animations
    ConnectionAnim connectionAnim;
    std::vector<TextParticle> textParticles;
    
    // Shape
    ShapeData currentShape;
    ShapeData nextShape;
    Vector2 cShapePos;

    // Stats
    float shapeMoveSpeed;
    float shapeFallSpeed;
    int level;
    Statistics stats;
};
