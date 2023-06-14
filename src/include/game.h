#pragma once
#include "app.h"
#include "shapes.h"
#include "animations.h"
#include "simulation.h"

class Application;
class Game;

// UI Constants
const int tileSize = 8;
const int boardWidth = 10;
const int boardHeight = 17;
const int panelBorderThickness = 6;
const int panelPadding = 32;
const int panelMarginSide = 16;
const int panelMarginTop = 12;
const float scale = 4;

Vector2 IndexToPos(int index, int sideLength);
void DrawBorder(Rectangle rect, int thickness, Color color);

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

struct TextParticle : Animation {
    std::string text;
    Vector2 startPos;
    float size;
    Color color;
    int offset;
};

Rectangle GetShapeRect(ShapeData shape);
void DrawShape(ShapeData shape, Vector2 pos, float scale);

class Game : Screen {
public:
    Game() = default;
    Game(Application* app) : app(app) {};

    void Load() override;
    void Update() override;
    
    void NewGame();
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
    void UpdateGameOverAnim();
    void UpdateTextParticles();

    bool IsShapeColliding();
    bool IsShapeInvalid();
    ShapeData NewShape();

    Simulation simulation;

private:
    int sinceSandUpdate;
    int bgAnimationTimer;
    bool gameOver = false;
    Timer bgAnimation;
    RenderTexture2D boardTex;
    Image blocksImg;
    Rectangle boardRect;
    Rectangle nextShapeRect;
    Rectangle infoPanelRect;
    Application* app;

    // Animations
    int gameOverTimer;
    ConnectionAnim connectionAnim;
    FallingParticleAnim gameOverParticleAnim;
    GameOverAnim gameOverAnim;
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
