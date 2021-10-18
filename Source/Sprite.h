#pragma once
#include <cstdint>

#define SPRITE_WIDTH     uint16_t(64)
#define SPRITE_HEIGHT    uint16_t(64)

#define SPRITE_TILES_X   uint16_t(SPRITE_WIDTH/16)
#define SPRITE_TILES_Y   uint16_t(SPRITE_HEIGHT)

#define SPRITE_BUF_SIZE  uint16_t(SPRITE_TILES_X*SPRITE_TILES_Y)

struct Sprite {
    uint16_t color[SPRITE_BUF_SIZE];
    uint16_t alpha[SPRITE_BUF_SIZE];

    Sprite();
    Sprite(const Sprite &);
};

