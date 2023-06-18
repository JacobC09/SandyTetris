#pragma once
#include <vector>

struct ShapeData {
    int type;
    int color;
    int style;
    int rotation;
};

struct Rotation {
    std::vector<bool> bitmap;
};

struct Shape {
    int size;
    std::vector<Rotation> rotations;
};

const int totalShapes = 8;
const int totalStyles = 5;
const int totalColors = 5;

const Shape shapeTypes[totalShapes] = {
    // T
    Shape {
        size: 3,
        rotations: {
            Rotation {
                bitmap: {
                    0, 1, 0,
                    1, 1, 1,
                    0, 0, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 0,
                    0, 1, 1,
                    0, 1, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 0, 0,
                    1, 1, 1,
                    0, 1, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 0,
                    1, 1, 0,
                    0, 1, 0
                }
            },
        }
    },
    // J
    Shape {
        size: 3,
        rotations: {
            Rotation {
                bitmap: {
                    1, 0, 0,
                    1, 1, 1,
                    0, 0, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 1,
                    0, 1, 0,
                    0, 1, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 0, 0,
                    1, 1, 1,
                    0, 0, 1
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 0,
                    0, 1, 0,
                    1, 1, 0
                }
            }
        }
    },
    // L
    Shape {
        size: 3,
        rotations: {
            Rotation {
                bitmap: {
                    0, 0, 1,
                    1, 1, 1,
                    0, 0, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 0,
                    0, 1, 0,
                    0, 1, 1
                }
            },
            Rotation {
                bitmap: {
                    0, 0, 0,
                    1, 1, 1,
                    1, 0, 0
                }
            },
            Rotation {
                bitmap: {
                    1, 1, 0,
                    0, 1, 0,
                    0, 1, 0
                }
            }
        }
    },
    // S
    Shape {
        size: 3,
        rotations: {
            Rotation {
                bitmap: {
                    0, 1, 1,
                    1, 1, 0,
                    0, 0, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 0,
                    0, 1, 1,
                    0, 0, 1
                }
            },
            Rotation {
                bitmap: {
                    0, 0, 0,
                    0, 1, 1,
                    1, 1, 0
                }
            },
            Rotation {
                bitmap: {
                    1, 0, 0,
                    1, 1, 0,
                    0, 1, 0
                }
            }
        }
    },
    // Z
    Shape {
        size: 3,
        rotations: {
            Rotation {
                bitmap: {
                    1, 1, 0,
                    0, 1, 1,
                    0, 0, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 0, 1,
                    0, 1, 1,
                    0, 1, 0
                }
            },
            Rotation {
                bitmap: {
                    0, 0, 0,
                    1, 1, 0,
                    0, 1, 1
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 0,
                    1, 1, 0,
                    1, 0, 0
                }
            }
        }
    },
    // O
    Shape {
        size: 2,
        rotations: {
            Rotation {
                bitmap: {
                    1, 1,
                    1, 1,
                }
            }
        }
    },
    // I
    Shape {
        size: 4,
        rotations: {
            Rotation {
                bitmap: {
                    0, 0, 0, 0,
                    1, 1, 1, 1,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                }
            },
            Rotation {
                bitmap: {
                    0, 0, 1, 0,
                    0, 0, 1, 0,
                    0, 0, 1, 0,
                    0, 0, 1, 0,
                },
            },
            Rotation {
                bitmap: {
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    1, 1, 1, 1,
                    0, 0, 0, 0,
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 0, 0,
                    0, 1, 0, 0,
                    0, 1, 0, 0,
                    0, 1, 0, 0,
                }
            }
        }
    },
    // Jake
    Shape {
        size: 3,
        rotations: {
            Rotation {
                bitmap: {
                    1, 1, 1,
                    0, 1, 0,
                    1, 1, 0,
                }
            },
            Rotation {
                bitmap: {
                    1, 0, 1,
                    1, 1, 1,
                    0, 0, 1,
                }
            },
            Rotation {
                bitmap: {
                    0, 1, 1,
                    0, 1, 0,
                    1, 1, 1,
                }
            },
            Rotation {
                bitmap: {
                    1, 0, 0,
                    1, 1, 1,
                    1, 0, 1,
                }
            },
        }
    }
};
