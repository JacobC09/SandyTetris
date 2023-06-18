#pragma once
#include <memory>
#include "common.h"
#include "pixelfont.h"
#include "transitions.h"

class Game;
class Intro;
class Ending;

const int screenWidth = 880;
const int screenHeight = 640;

namespace Colors {
    const Color orange0 = Color {139, 28, 3, 255};
    const Color orange1 = Color {181, 64, 25, 255};
    const Color orange2 = Color {225, 110, 51, 255};
    const Color orange3 = Color {231, 128, 54, 255};
    const Color orange4 = Color {250, 136, 65, 255};
    const Color dim = ColorAlpha(BLACK, 0.9);
}

class Screen {
public:
    virtual void Load() = 0;
    virtual void Update() = 0;
};

class Application {
public:
    enum class States {
        Intro,
        Game,
        Ending
    };

    struct Settings {
        bool music = true;
        bool sfx = true;
    };

    Game* game;
    Intro* intro;
    Ending* ending;
    Music music;
    States state;
    Settings settings;
    PixelFont font;
    Application() = default;
    
    void Load();
    void Run();
    void Unload();
    
    template<typename Transition, typename... Args>
    void AddTransition(std::string name, Args&&... args) {
        transitions[name] = std::make_unique<Transition>(args...);
    }

    Transition* GetTransition(std::string name) {
        auto pos = transitions.find(name);
        if (pos != transitions.end()) {
            return pos->second.get();
        } else {
            return nullptr;
        }
    }

private:
    int time;
    int iResoluationLoc;
    int iTimeLoc;
    std::map<std::string, std::unique_ptr<Transition>> transitions;
};
