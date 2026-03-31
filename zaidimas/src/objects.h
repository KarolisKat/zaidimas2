#pragma once
#include <SFML/Graphics.hpp>
#include "defines.h"

struct Player {
    float x, y;
    int lives;

    // Ðiø funkcijø ieðko tavo utils.cpp (C2039 klaidos)
    float LegsStartX() { return x + 25.0f; }
    float LegsEndX() { return x + 55.0f; }
    float LegsY() { return y + 73.0f; }
};

struct Plate {
    float x, y;
    int type;

    // Ðiø funkcijø ieðko tavo utils.cpp (C2039 klaidos)
    float StartX() { return x; }
    float EndX() { return x + 60.0f; }
    float TopY() { return y; }
    float BottomY() { return y + 100.0f; }
};