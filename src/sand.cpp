#include "sand.h"

Simulation::Simulation(int width, int height) : width(width), height(height) {
    sandBuffer = std::unique_ptr<SandParticle[]>(new SandParticle[width * height]);
    Reset();
}

void Simulation::Step() {
    for (int y = height - 2; y > -1; y--) {
        for (int x = 0; x < width; x++) {
            SandParticle particle = *GetAt(x, y);
            if (!particle.occupied) continue;

            // Can Move Down?
            if (!GetAt(x, y + 1)->occupied) {
                SetAt(x, y, SandParticle{});
                SetAt(x, y + 1, particle);
                continue;
            }

            // Can Move Side?
            if (x + 1 < width && !GetAt(x + 1, y + 1)->occupied && !GetAt(x + 1, y)->occupied) {
                SetAt(x, y, SandParticle {});
                SetAt(x + 1, y + 1, particle);
                continue;
            }
            
            if (x > 0 && !GetAt(x - 1, y + 1)->occupied && !GetAt(x - 1, y)->occupied) {
                SetAt(x, y, SandParticle {});
                SetAt(x - 1, y + 1, particle);
            }
        }
    }
}

SandParticle* Simulation::GetAt(int x, int y) {
    if (!ValidPosition(x, y)) {
        return nullptr;
    }

    return &sandBuffer.get()[IndexAt(x, y)];
}

void Simulation::SetAt(int x, int y, SandParticle value) {
    if (!ValidPosition(x, y)) {   
        return;
    }
    
    sandBuffer.get()[IndexAt(x, y)] = value;
}

void Simulation::Reset() {
    for (int i = 0; i < width * height; i++) {
        sandBuffer.get()[i] = SandParticle{false};
    }
}

void Simulation::ResetVisited() {
    for (int i = 0; i < width * height; i++) {
        sandBuffer.get()[i].visited = false;
    }
}

int Simulation::IndexAt(int x, int y) {
    return y * width + x;
}

bool Simulation::ValidPosition(int x, int y) {
    return (x >= 0 && x < width && y >= 0 and y < height);
}
