#include "raylib.h"

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

	Texture wabbit = LoadTexture("wabbit_alpha.png");

	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(BLACK);

		DrawText("Hello Raylib", 200, 200, 20, WHITE);

		DrawTexture(wabbit, 400, 200, WHITE);

		EndDrawing();
	}

	UnloadTexture(wabbit);

	CloseWindow();
	return 0;
}
