#include "lib.h"
#include "render.h"
#include "emulator.h"

#ifdef PLATFORM_WEB
#include <emscripten.h>
#endif

#define SUPPORT_MODULE_RAUDIO

#include "resource_dir.h"

chip8_t chip8 = {
	.state = NOT_INITIALIZED,
};

menu_t main_menu = {
	.state = MAIN_MENU,
};

// TODO:
//  - Back to main menu
//  - Reset games if win or lose

void update_draw_frame(void);
void clean_up_menu(menu_t *menu);
void clean_up_menu_item(menu_item_t *item);

int main()
{
	init_display();

	SearchAndSetResourceDir("resources");

#ifdef PLATFORM_WEB
	emscripten_set_main_loop(update_draw_frame, 0, 1);
#else
	SetTargetFPS(60);
	TraceLog(LOG_INFO, "Chip 8 State: %d", chip8.state);
	while (chip8.state != QUIT)
	{
		update_draw_frame();
	}
#endif

	clean_up_menu(&main_menu);
	UnloadSound(chip8.beep);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}

void update_draw_frame(void)
{
	BeginDrawing();
	switch (main_menu.state)
	{
	case EMULATOR:
		if (chip8.state == STARTED)
		{
			if (!chip8_init(&chip8, main_menu.items[main_menu.current_selected_item_index].roms))
			{
				CloseWindow();
				return;
			}
			load_font(&chip8);

			audio_init(&chip8);
		}
		if (chip8.state == PAUSED)
			return;

		action_key(&chip8);
		ClearBackground(BLACK);

		const int INSTRUCTION_PER_FRAME = 700 / 60;
		for (int i = 0; i < INSTRUCTION_PER_FRAME; ++i)
		{
			emulator_instruction_cycle(&chip8);
		}

		render_display(&chip8);
		update_timers(&chip8);

		break;

	case MAIN_MENU:
		if (IsFileDropped())
		{
			utility_drag_drop_rom(&main_menu);
		}
		ClearBackground(BLACK);
		render_main_menu(&main_menu, &chip8);
		break;
	default:
		DrawText("Unknown display state", 10, 10, 20, RAYWHITE);
		break;
	}

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