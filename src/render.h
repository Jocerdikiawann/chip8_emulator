#ifndef RENDER_H
#define RENDER_H

#include "emulator.h"
#include "lib.h"

#define SCALE_FACTOR 10
#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SCREEN_WIDTH (CHIP8_WIDTH * SCALE_FACTOR)
#define SCREEN_HEIGHT (CHIP8_HEIGHT * SCALE_FACTOR)

void init_display();
void render_display(chip8_t *chip8);

#endif