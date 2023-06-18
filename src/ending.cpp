#include "ending.h"

const std::string endingText = "Thanks for playing Sandy Tetris";
const std::vector<std::string> endingTextColors = {"Thanks for playing ", "Sandy Tetris"};

void Ending::Load() {
    inbetweenTimer = 0;

    std::vector<std::vector<std::string>> lines = {
        {"You have completed", " Sandy Tetris"},
        {"Congrats!", " :D"},
        {"Why have you spent your", " time", " doing this"},
        {"Don't you have", " better", " things to do"},
        {"Like going", " outside"},
        {"Seriously..."},
        {"How", " sad", " is your life"},
        {"I suggest you seek", " mental aide"},
        {"This is a serious", " low point", " for you"},
        {"You might be", " wondering..."},
        {"How many", " text dialogs", " are there?"},
        {"This is the", " last", " one"},
        {"I swear"},
        {"I", " wouldn't", " lie"},
        {"Just kidding :D"},
        {"Ha you got", " fooled", " :D"},
        {"Although, seriously the game is", " over", " :|"},
        {"Leave!"},
        {"Why are you", " still", " here :("},
        {"I told you to", " leave"},
        {"There is no", " end credit scene"},
        {"The game is", " over"},
        {"LEAVE!!!!!"},
        {"I'm not going to stop until you", " leave"},
        {"If you don't leave in", " 10", " seconds..."},
        {"I will", " explode", " your computer"},
        {"Trust me you", " don't", " want that"},
        {"Computers are", " expensive"},
        {"10"},
        {"9"},
        {"8"},
        {"7"},
        {"6"},
        {"5"},
        {"4"},
        {"3"},
        {"2"},
        {"1"},
        {"0.5"},
        {"0.25"},
        {"0.1"},
        {"0.01"},
        {"0.00001"},
        {"Come on man"},
        {"Why won't you", " go away"},
        {"Although I respect your", " commitment"},
        {"Patients is a virtue"},
        {"This is your", " last warning"},
        {"Bye bye :D"},
    };

    std::vector<std::vector<Color>> colors = {
        {WHITE, Colors::orange3},
        {Colors::orange3, WHITE},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3},
        {Colors::orange3},
        {WHITE, BLUE, WHITE},
        {WHITE, Colors::orange3},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3, WHITE},
        {LIGHTGRAY},
        {WHITE, Colors::orange3, WHITE},
        {Colors::orange3},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3, WHITE},
        {RED},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3},
        {WHITE, Colors::orange3},
        {WHITE, Colors::orange3},
        {RED},
        {WHITE, RED},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3, WHITE},
        {WHITE, Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {Colors::orange3},
        {RED},
        {WHITE, RED},
        {WHITE, Colors::orange3},
        {LIGHTGRAY},
        {WHITE, RED},
        {Colors::orange3},
    };

    for (int i = 0; i < (signed) std::min(lines.size(), colors.size()); i++) {
        texts.push_back(FadeInCenterText {
            .font = &app->font,
            .texts = lines[i],
            .colors = colors[i],
            .size = 2,
            .fadeInTime = 90,
            .stayTime = 20,
            .fadeOutTime = 60,
        });
    }

    texts.back().stayTime = -1;
}

void Ending::Update() {
    ClearBackground({12, 5, 1, 255});

    if (inbetweenTimer) {
        inbetweenTimer--;
    } else if (texts.size()) {
        FadeInCenterText &animation = texts.front();
        animation.update();
        if (animation.finished) {
            texts.erase(texts.begin());
            inbetweenTimer = 10;
        }
    }
}