#pragma once
#include "common.h"

class PixelFont {
public:
    int spaceSize;
    int letterDistance = 1;
    int lineOffset = 0;
    float height;
    Texture2D texture;
    std::map<char, Rectangle> letters;

    PixelFont() = default;
    PixelFont(Texture2D text, std::string chars, int _spaceSize, Color splitColor = BLACK);

    int Measure(std::string text);
    void SetValues(int _letterDistance, int _lineOffset);

    void Render(std::string text, Vector2 pos, float size, Color color);
    void RenderCentered(std::string text, Vector2 pos, int size, Color color, bool centerX=false, bool centerY=false);
    void RenderCenteredRec(Rectangle region, std::string text, int size, Color color);
    
    void RenderColored(std::vector<std::string> texts, Vector2 pos, float size, std::vector<Color> colors);
};