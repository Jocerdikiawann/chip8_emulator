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
void clean_up_menu(menu_t *menu);
void clean_up_menu_item(menu_item_t *item);

int main()
{
	init_display();

	SearchAndSetResourceDir("resources");

	// Load the main menu
	menu_t main_menu = {
		.items = NULL,
		.count = 0,
		.capacity = 0,
	};

	while (!WindowShouldClose())
	{
		if (IsFileDropped())
		{
			utility_drag_drop_rom(&main_menu);
		}
		BeginDrawing();
		ClearBackground(BLACK);
		render_main_menu(&main_menu);
		EndDrawing();
	}

	clean_up_menu(&main_menu);
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

void clean_up_menu(menu_t *menu)
{
	for (size_t i = 0; i < menu->count; ++i)
	{
		clean_up_menu_item(&menu->items[i]);
	}
	free(menu->items);
	menu->items = NULL;
	menu->capacity = 0;
	menu->count = 0;
}

void clean_up_menu_item(menu_item_t *item)
{
	free(item->roms);
	item->roms = NULL;
	item->roms_name = NULL;
}