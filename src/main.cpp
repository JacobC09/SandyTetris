#include "app.h"

int main() {
    Application* app = new Application();

    app->Load();
    app->Run();
    app->Unload();

    delete app;
}
