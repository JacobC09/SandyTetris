#pragma once
#include "common.h"

// Textures
enum class Textures {
    desertBg1,
    desertBg2,
    blocks,
    font
};

inline std::map<Textures, const char*> texturePaths = {
    {Textures::desertBg1, "assets/image/desertbg1.png"},
    {Textures::desertBg2, "assets/image/desertbg2.png"},
    {Textures::blocks, "assets/image/blocks.png"},
    {Textures::font, "assets/image/font.png"},
};

// Sounds
enum class Sounds {
    block_fall,
    block_rotate,
    clear_1,
    clear_2,
    clear_3,
    clear_4,
    clear_5,
    clear_6,
    game_over_1,
    game_over_2,
    level_up
};

inline std::map<Sounds, const char*> soundPaths = {
    {Sounds::block_fall, "assets/sfx/block_fall.mp3"},
    {Sounds::block_rotate, "assets/sfx/block_rotate.mp3"},
    {Sounds::clear_1, "assets/sfx/clear_1.mp3"},
    {Sounds::clear_2, "assets/sfx/clear_2.mp3"},
    {Sounds::clear_3, "assets/sfx/clear_3.mp3"},
    {Sounds::clear_4, "assets/sfx/clear_4.mp3"},
    {Sounds::clear_5, "assets/sfx/clear_5.mp3"},
    {Sounds::clear_6, "assets/sfx/clear_6.mp3"},
    {Sounds::game_over_1, "assets/sfx/game_over_1.mp3"},
    {Sounds::game_over_2, "assets/sfx/game_over_2.mp3"},
    {Sounds::level_up, "assets/sfx/level_up.mp3"},
};

// Shaders
enum class Shaders {
    Heat
};

#ifdef PLATFORM_WEB
    inline std::map<Shaders, const char*> shaderPaths = {
        {Shaders::Heat, "assets/shader/100/heat.fs"},
    };
#else
    inline std::map<Shaders, const char*> shaderPaths = {
        {Shaders::Heat, "assets/shader/330/heat.fs"},
    };
#endif

void LoadAssets();
void UnloadAssets();

Texture2D &GetTexture(Textures texture);
Sound &GetSound(Sounds sound);
Shader &GetShader(Shaders shader);
