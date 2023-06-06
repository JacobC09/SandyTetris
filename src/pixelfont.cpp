#include <cmath>
#include "pixelfont.h"

PixelFont::PixelFont(Texture2D text, std::string chars, int _spaceSize, Color splitColor) {
    texture = text;
    spaceSize = _spaceSize;

    height = (float) text.height;

    int characterIndex = 0;
    int begin = 0;
    int current = 1;
    Image image = LoadImageFromTexture(text);
    while (current <= text.width && characterIndex < (signed) chars.size()) {
        bool isSplitColor = true;
        if (current < text.width) {
            Color pixelColor = GetImageColor(image, current, 0);
            isSplitColor = pixelColor.r == splitColor.r && pixelColor.g == splitColor.g && pixelColor.b == splitColor.b && pixelColor.a == splitColor.a;
        }

        if (isSplitColor) {
            if (current > begin) {
                letters[chars[characterIndex]] = Rectangle {(float) begin, 0, (float) current - begin, (float) text.height};
                current++;
                characterIndex++;
                begin = current;
            }
        }
        current++;
    }
}

int PixelFont::Measure(std::string text) {
    int width = 0;
    for (char character : text) {
        if (character == '\n') {
            width = 0;
        } else if (character == ' ') {
            width += spaceSize;
        } else if (character == '\t') {
            width += spaceSize * 4;
        } else if (letters.find(character) != letters.end()) {
            width += letters[character].width + letterDistance;
        }
    }
    return width;
}

void PixelFont::SetValues(int _letterDistance, int _lineOffset) {
    letterDistance = _letterDistance;
    lineOffset = _lineOffset;
}

void PixelFont::Render(std::string text, Vector2 pos, float size, Color color) {
    pos = {std::floor(pos.x), std::floor(pos.y)};

    for (char character : text) {
        if (character == '\n') {
            pos.x = 0;
            pos.y += (texture.height + lineOffset) * size;
        } else if (character == ' ') {
            pos.x += spaceSize * size;
        } else if (character == '\t') {
            pos.x += spaceSize * 4 * size;
        } else if (letters.find(character) != letters.end()) {
            Rectangle source = letters[character];
            Rectangle dest = {pos.x, pos.y, source.width * size, source.height * size};
            DrawTexturePro(texture, source, dest, {0, 0}, 0, color);
            pos.x += (source.width + letterDistance) * size;
        }
    }
}

void PixelFont::RenderCentered(std::string text, Vector2 pos, int size, Color color, bool centerX, bool centerY) {
    if (centerX)
        pos.x -= (Measure(text) * size) / 2;
    if (centerY)
        pos.y -= (height * size) / 2;

    Render(text, pos, size, color);
}

void PixelFont::RenderCenteredRec(Rectangle region, std::string text, int size, Color color) {
    Render(text, {
        region.x + region.width / 2 - Measure(text) * size / 2, 
        region.y + region.height / 2 - height * size / 2 - (size - 1)
    }, size, color);
}

void PixelFont::RenderColored(std::vector<std::string> texts, Vector2 pos, float size, std::vector<Color> colors) {
    for (int i = 0; i < (signed) std::min(texts.size(), colors.size()); i++) {
        Render(texts[i], pos, size, colors[i]);
        pos.x += Measure(texts[i]) * size;
    }
}
