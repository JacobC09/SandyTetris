#pragma once
#include "common.h"
#include "game.h"
#include "pixelfont.h"
#include "transitions.h"

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
    PixelFont font;
    std::map<std::string, std::unique_ptr<Transition>> transitions;
    
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
};
