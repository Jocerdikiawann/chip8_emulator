#ifndef RENDER_H
#define RENDER_H

#include "emulator.h"
#include "lib.h"

#define SCALE_FACTOR 10
#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SCREEN_WIDTH (CHIP8_WIDTH * SCALE_FACTOR)
#define SCREEN_HEIGHT (CHIP8_HEIGHT * SCALE_FACTOR)

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



void init_display();
void render_main_menu(menu_t *menu, chip8_t *chip8);
void utility_drag_drop_rom(menu_t *menu);
void render_display(chip8_t *chip8);

#endif