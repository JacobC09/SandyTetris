#pragma once

struct Level {
    float fallSpeed;
    float horizontalSpeed;
    int maxColors;
    int requiredClears;
};

const int maxLevels = 6;

const Level levels[maxLevels] = {
    Level {
        .fallSpeed = 1,
        .horizontalSpeed = 2,
        .maxColors = 2,
        .requiredClears = 2,
    },
    Level {
        .fallSpeed = 1.2,
        .horizontalSpeed = 2.1,
        .maxColors = 3,
        .requiredClears = 8,
    },
    Level {
        .fallSpeed = 1.5,
        .horizontalSpeed = 2.2,
        .maxColors = 4,
        .requiredClears = 12,
    },
    Level {
        .fallSpeed = 2,
        .horizontalSpeed = 2.4,
        .maxColors = 4,
        .requiredClears = 16,
    },
    Level {
        .fallSpeed = 2.4,
        .horizontalSpeed = 2.5,
        .maxColors = 4,
        .requiredClears = 24,
    },
    Level {
        .fallSpeed = 3,
        .horizontalSpeed = 2.6,
        .maxColors = 5,
        .requiredClears = 32,
    }
};