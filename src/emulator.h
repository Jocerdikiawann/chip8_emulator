#ifndef EMULATOR_H
#define EMULATOR_H

#include "lib.h"

bool chip8_init(chip8_t *chip8, const char *filename);
void audio_init(chip8_t *chip8);
void load_font(chip8_t *chip8);
void action_key(chip8_t *chip8, menu_t *menu);
void emulator_instruction_cycle(chip8_t *chip8);
void update_timers(chip8_t *chip8);

#endif