#include <memory>
#include "common.h"

struct SandParticle {
    bool occupied = false;
    Color color;
    int type;
    bool visited = false;
};

class Simulation {
public:
    Simulation() = default;
    Simulation(int width, int height);

    void Step();

    SandParticle* GetAt(int x, int y);
    void SetAt(int x, int y, SandParticle value);
    void Reset();
    void ResetVisited();
    int IndexAt(int x, int y);
    bool ValidPosition(int x, int y);

    int width;
    int height;

private:
    std::unique_ptr<SandParticle[]> sandBuffer;
};
