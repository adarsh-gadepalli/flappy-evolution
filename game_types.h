#ifndef GAME_TYPES_H
#define GAME_TYPES_H

struct Bird {
    float x, y;
    float vx, vy;
};

struct Pipe {
    float x;
    float gap;
    float gapY;
    bool passed = false;
};

enum class GameState {
    START,
    PLAYING
};

// Game constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PIPE_WIDTH = 60.0f;
const float BIRD_SIZE = 20.0f;
const float GRAVITY = 0.5f;
const float JUMP_VELOCITY = -8.0f;
const float SCROLL_SPEED = 2.0f;

#endif