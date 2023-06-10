#pragma once
#include "common.h"

// Textures
enum class Textures {
    desertBg1,
    desertBg2,
    blocks
};

inline std::map<Textures, const char*> texturePaths = {
    {Textures::desertBg1, "assets/image/desertbg1.png"},
    {Textures::desertBg2, "assets/image/desertbg2.png"},
    {Textures::blocks, "assets/image/blocks.png"},
};

// Sounds
enum class Sounds {
    gerudoValley
};

inline std::map<Sounds, const char*> soundPaths = {
    {Sounds::gerudoValley, "assets/sound/Gerudo Valley 8 Bit.mp3"},
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
