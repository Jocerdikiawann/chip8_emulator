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
    case 0x0000:
        switch (chip8->instruction.opcode & 0x00FF)
        {
        case 0x00E0:
            chip8->instruction = (emulator_instruction_t){
                .opcode = chip8->instruction.opcode,

            };
            break;
        case 0x00EE:
            chip8->instruction = (emulator_instruction_t){
                .opcode = chip8->instruction.opcode,

            };
            break;
        default:
            chip8->instruction = (emulator_instruction_t){
                .opcode = chip8->instruction.opcode,
            };
            break;
        }
        break;
    case 0x1000:
        chip8->instruction = (emulator_instruction_t){
            .opcode = chip8->instruction.opcode,

        };
        break;
    case 0x2000:
        chip8->instruction = (emulator_instruction_t){
            .opcode = chip8->instruction.opcode,

        };
        break;
    case 0x3000:
        chip8->instruction = (emulator_instruction_t){
            .opcode = chip8->instruction.opcode,

        };
        break;
    case 0x4000:
        chip8->instruction = (emulator_instruction_t){
            .opcode = chip8->instruction.opcode,

        };
        break;
    case 0x5000:
        chip8->instruction = (emulator_instruction_t){
            .opcode = chip8->instruction.opcode,

        };
        break;
    default:
        break;
    };
}