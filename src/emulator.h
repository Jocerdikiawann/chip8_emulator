#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"

typedef enum
{
    QUIT,
    RUNNING,
    PAUSED
} emultaor_state_t;

typedef struct
{
    char *rom_name;
    unsigned int rom_size;
} rom_t;

typedef struct
{
    uint8_t registers[16]; // 16 8-bit registers
    uint8_t memory[4096];  // 4KB of memory
    uint16_t I;            // index register
    uint16_t pc;           // program counter
    uint16_t stack[16];    // 16 levels of stack
    uint8_t sp;            // stack pointer
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[16];                   // keypad
    uint32_t display_resolution[64 * 32]; // original display resolution CHIP8 is 64x32 pixels
    uint16_t opcode;                      // current opcode
    emultaor_state_t state;
} chip8_t;

bool chip8_init(chip8_t *chip8, rom_t *rom, const char *filename);
void load_font(chip8_t *chip8);
void action_key(chip8_t *chip8);

#endif