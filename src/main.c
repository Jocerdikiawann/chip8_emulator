#include "lib.h"
#include "render.h"
#include "emulator.h"

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#define SUPPORT_MODULE_RAUDIO

#include "resource_dir.h"

chip8_t chip8;
rom_t rom;

void update_draw_frame(void);

int main()
{
	init_display();

	SearchAndSetResourceDir("resources");

	Vector2 pos_box = {
		.x = GetScreenWidth() * 0.5f,
		.y = GetScreenHeight() * 0.5f,
	};

	float box_width = GetScreenWidth() - (40 * 2);
	float box_height = GetScreenHeight() - (40 * 2);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);
		DrawRectangleLines(
			pos_box.x - (box_width / 2),
			pos_box.y - (box_height / 2),
			box_width,
			box_height,
			GRAY);

		for (size_t i = 0; i < 5; i++)
		{
			char *text = TextFormat("Item %i", i);
			Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
			Vector2 text_pos = {
				.x = pos_box.x - text_size.x * 0.5f,
				.y = pos_box.y - box_height * 0.5f + 20 + (i * 40),
			};
			DrawText(text, text_pos.x, text_pos.y, 20, WHITE);
		}
		EndDrawing();
	}

	// if (!chip8_init(&chip8, &rom, "tetris.ch8"))
	//{
	// CloseWindow();
	// return 1;
	//}
	// load_font(&chip8);

	// audio_init(&chip8);

	// #ifdef PLATFORM_WEB
	// emscripten_set_main_loop(update_draw_frame, 0, 1);
	// #else
	// SetTargetFPS(60);
	// while (chip8.state != QUIT)
	//{
	// update_draw_frame();
	//}
	// #endif

	// UnloadSound(chip8.beep);
	// CloseAudioDevice();
	// free(rom.rom_name);
	// CloseWindow();
	return 0;
}

void update_draw_frame(void)
{
	if (chip8.state == PAUSED)
		return;

	BeginDrawing();
	ClearBackground(BLACK);

	// TODO: Create main menu before starting the game
	// TODO: - List of games
	// TODO: - Reset games if game over or win
	// TODO: - Drag and drop Rom files

	// Start Emulator section
	action_key(&chip8);

	const int INSTRUCTION_PER_FRAME = 700 / 60;
	for (int i = 0; i < INSTRUCTION_PER_FRAME; ++i)
	{
		emulator_instruction_cycle(&chip8);
	}

	render_display(&chip8);
	update_timers(&chip8);
	// End of emulator section

	EndDrawing();
}