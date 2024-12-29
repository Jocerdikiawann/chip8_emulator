#include "emulator.h"

bool chip8_init(chip8_t *chip8, rom_t *rom, const char *filename)
{
    TraceLog(LOG_INFO, "Initializing CHIP-8 emulator");
    memset(chip8, 0, sizeof(chip8_t));
    const uint32_t entry_point = 0x200; // CHIP-8 programs start at 0x200

    chip8->pc = entry_point;

    unsigned int data_size = 0;
    unsigned char *file = LoadFileData(filename, &data_size);

    if (file == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to load file: %s", filename);
        return false;
    }

    if (data_size > (sizeof(chip8->memory) - entry_point))
    {
        TraceLog(LOG_ERROR, "File too large: %s", filename);
        UnloadFileData(file);
        return false;
    }

    memcpy(&chip8->memory[entry_point], file, data_size);
    UnloadFileData(file);

    rom->rom_name = strdup(filename);
    rom->rom_size = data_size;

    chip8->state = RUNNING;

    TraceLog(LOG_INFO, "CHIP-8 emulator initialized");
    return true;
}

void load_font(chip8_t *chip8)
{
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memcpy(&chip8->memory[0], font, sizeof(font));
}

void action_key(chip8_t *chip8)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        chip8->state = QUIT;
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        if (chip8->state == PAUSED)
        {
            chip8->state = RUNNING;
        }
        else
        {
            chip8->state = PAUSED;
            TraceLog(LOG_INFO, "Paused");
        }
    }
}

void emulator_instruction_cycle(chip8_t *chip8)
{
    chip8->instruction.opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    chip8->pc += 2;

    chip8->instruction.NNN = chip8->instruction.opcode & 0x0FFF;
    chip8->instruction.NN = chip8->instruction.opcode & 0x0FF;
    chip8->instruction.N = chip8->instruction.opcode & 0x0F;
    chip8->instruction.X = (chip8->instruction.opcode >> 8) & 0x0F;
    chip8->instruction.Y = (chip8->instruction.opcode >> 4) & 0x0F;

    switch ((chip8->instruction.opcode >> 12) & 0x0F)
    {
    case 0x00:
        if (chip8->instruction.NN == 0xE0)
        {
            // Clear the display
            memset(chip8->display_resolution, 0, sizeof(chip8->display_resolution));
        }
        else if (chip8->instruction.NN == 0xEE)
        {
            // Return from a subroutine
            chip8->pc = *--chip8->sp;
        }
        break;
    case 0x01:
        // Jump to address NNN
        chip8->pc = chip8->instruction.NNN;
        break;
    case 0x02:
        // Call subroutine at NNN
        *chip8->sp++ = chip8->pc;
        chip8->pc = chip8->instruction.NNN;
        break;
    case 0x03:
        // Set register X to NN
        chip8->registers[chip8->instruction.X] = chip8->instruction.NN;
        break;
    case 0x04:
        // chip8->I = chip8->instruction.NNN;
        break;
    case 0x05:

        break;
    case 0x06:
        chip8->registers[chip8->instruction.X] = chip8->instruction.NN;
        break;
    case 0x07:
        chip8->registers[chip8->instruction.X] += chip8->instruction.NN;
        break;
    case 0x08:

        break;
    case 0x09:

        break;
    case 0x0A:
        chip8->I = chip8->instruction.NNN;
        break;
    case 0x0B:

        break;
    case 0x0C:

        break;
    case 0x0D:
        int video_width = 64;
        int video_height = 32;

        uint8_t x_pos = chip8->registers[chip8->instruction.X] % video_width;
        uint8_t y_pos = chip8->registers[chip8->instruction.Y] % video_height;

        // VF
        chip8->registers[0xF] = 0;

        for (size_t row = 0; row < chip8->instruction.N; ++row)
        {
            if (y_pos + row >= video_height)
                break;

            uint8_t sprite_byte = chip8->memory[chip8->I + row];
            for (size_t col = 0; col < 8; ++col)
            {
                if (x_pos + col >= video_width)
                    break;
                uint8_t sprite_pixel = sprite_byte & (0x80 >> col);
                if (sprite_pixel)
                {
                    size_t display_pixel_index = (y_pos + row) * video_width + (x_pos + col);
                    if (chip8->display_resolution[display_pixel_index] == 1)
                    {
                        chip8->registers[0xF] = 1;
                    }

                    chip8->display_resolution[display_pixel_index] ^= 1;
                }
            }
        }
        break;
    case 0x0E:

        break;
    case 0x0F:

        break;
    default:
        break;
    };
}