#ifndef EMULATOR_H
#define EMULATOR_H

#include "lib.h"

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
    uint16_t opcode;
    uint16_t NNN;
    uint8_t NN;
    uint8_t N;
    uint8_t X;
    uint8_t Y;
} emulator_instruction_t;

typedef struct
{
    uint8_t registers[16]; // 16 8-bit registers
    uint8_t memory[4096];  // 4KB of memory
    uint16_t I;            // index register
    uint16_t pc;           // program counter
    uint16_t stack[16];    // 16 levels of stack
    uint16_t *sp;          // stack pointer
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[16];      // keypad
    uint32_t video[64 * 32]; // original display resolution CHIP8 is 64x32 pixels
    emulator_instruction_t instruction;
    emultaor_state_t state;
} chip8_t;

bool chip8_init(chip8_t *chip8, rom_t *rom, const char *filename);
void load_font(chip8_t *chip8);
void action_key(chip8_t *chip8);
void emulator_instruction_cycle(chip8_t *chip8);

#endif