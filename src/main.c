#include "lib.h"
#include "render.h"
#include "emulator.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "resource_dir.h"

chip8_t chip8;
rom_t rom;

void update_draw_frame(void)
{
	BeginDrawing();
	ClearBackground(BLACK);

	action_key(&chip8);

	const int INSTRUCTION_PER_FRAME = 700 / 60;
	for (int i = 0; i < INSTRUCTION_PER_FRAME; ++i)
	{
		emulator_instruction_cycle(&chip8);
	}

	render_display(&chip8);
	update_timers(&chip8);
	EndDrawing();
}

int main()
{
	init_display();

	SearchAndSetResourceDir("resources");

	if (!chip8_init(&chip8, &rom, "brix.ch8"))
	{
		CloseWindow();
		return 1;
	}
	load_font(&chip8);

	audio_init(&chip8);

#ifdef PLATFORM_WEB
	emscripten_set_main_loop(update_draw_frame, 0, 1);
#else
	while (chip8.state != QUIT)
	{
		update_draw_frame();
	}
#endif

	UnloadSound(chip8.beep);
	CloseAudioDevice();
	free(rom.rom_name);
	CloseWindow();
	return 0;
}
