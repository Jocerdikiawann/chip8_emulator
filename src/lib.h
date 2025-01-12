#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"

typedef enum
{
    QUIT,
    RUNNING,
    NOT_INITIALIZED,
    PAUSED,
    STARTED,
} emultaor_state_t;

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
    uint16_t keypad[16];     // keypad
    uint32_t video[64 * 32]; // original display resolution CHIP8 is 64x32 pixels
    emulator_instruction_t instruction;
    emultaor_state_t state;
    Sound beep;
    bool sound_playing;
} chip8_t;

typedef struct
{
    char *roms;
    char *roms_name;
    bool is_selected;
    Vector2 position;
} menu_item_t;

typedef enum
{
    MAIN_MENU,
    EMULATOR
} display_state_t;

typedef struct
{
    menu_item_t *items;
    size_t current_selected_item_index;
    size_t count;
    size_t capacity;
    display_state_t state;
} menu_t;

#endif