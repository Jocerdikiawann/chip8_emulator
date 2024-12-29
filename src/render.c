#include "render.h"

void init_display()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    int display = GetCurrentMonitor(), full_width = GetMonitorWidth(display),
        full_height = GetMonitorHeight(display);

    int factor = 70;
    full_width = factor * 16;
    full_height = factor * 9;

    InitWindow(full_width, full_height, "CHIP-8 Emulator");

    SetWindowMinSize(320, 160);
    SetTargetFPS(60);
}

void render_display(chip8_t *chip8)
{
    int window_width = GetScreenWidth();
    int window_height = GetScreenHeight();

    float target_aspect = 2.0f; // 64/32
    float current_aspect = (float)window_width / window_height;

    int display_width, display_height;
    if (current_aspect > target_aspect)
    {
        display_height = window_height;
        display_width = window_height * target_aspect;
    }
    else
    {
        display_width = window_width;
        display_height = window_width / target_aspect;
    }

    int offset_x = (window_width - display_width) / 2;
    int offset_y = (window_height - display_height) / 2;

    float pixel_width = (float)display_width / CHIP8_WIDTH;
    float pixel_height = (float)display_height / CHIP8_HEIGHT;

    for (int y = 0; y < CHIP8_HEIGHT; ++y)
    {
        for (int x = 0; x < CHIP8_WIDTH; ++x)
        {
            if (chip8->display_resolution[y * CHIP8_WIDTH + x])
            {
                DrawRectangle(
                    offset_x + (x * pixel_width),
                    offset_y + (y * pixel_height),
                    ceil(pixel_width),
                    ceil(pixel_height),
                    WHITE);
            }
        }
    }
}