#include "lib.h"
#include "render.h"
#include "emulator.h"

#include "resource_dir.h"

int main()
{
	init_display();

	SearchAndSetResourceDir("resources");

	chip8_t chip8;
	rom_t rom;
	if (!chip8_init(&chip8, &rom, "test_opcode.ch8"))
	{
		CloseWindow();
		return 1;
	}

	while (chip8.state != QUIT)
	{
		BeginDrawing();
		ClearBackground(BLACK);

		action_key(&chip8);

		if (chip8.state == PAUSED)
			continue;

		const int INSTRUCTION_PER_FRAME = 700 / 60;
		for (int i = 0; i < INSTRUCTION_PER_FRAME; ++i)
		{
			emulator_instruction_cycle(&chip8);
		}

		render_display(&chip8);
		update_timers(&chip8);
		EndDrawing();
	}

	free(rom.rom_name);
	CloseWindow();
	return 0;
}
