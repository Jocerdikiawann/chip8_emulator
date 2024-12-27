#include "raylib.h"
#include "emulator.h"

#include "resource_dir.h"

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	int display = GetCurrentMonitor(), full_width = GetMonitorWidth(display),
		full_height = GetMonitorHeight(display);

	int factor = 70;
	full_width = factor * 16;
	full_height = factor * 9;

	InitWindow(full_width, full_height, "Hello Raylib");

	SearchAndSetResourceDir("resources");

	chip8_t chip8;
	rom_t rom;
	if (!chip8_init(&chip8, &rom, "ibm.ch8"))
	{
		CloseWindow();
		return 1;
	}

	while (chip8.state != QUIT)
	{
		BeginDrawing();

		action_key(&chip8);

		if(chip8.state == PAUSED) continue;

		ClearBackground(BLACK);

		DrawText("Hello Raylib", 200, 200, 20, WHITE);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
