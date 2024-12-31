#include "emulator.h"

bool chip8_init(chip8_t *chip8, rom_t *rom, const char *filename)
{
    TraceLog(LOG_INFO, "Initializing CHIP-8 emulator");
    memset(chip8, 0, sizeof(chip8_t));
    const uint32_t entry_point = 0x200; // CHIP-8 programs start at 0x200

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

    chip8->pc = entry_point;
    chip8->sp = &chip8->stack[0];
    chip8->state = RUNNING;

    rom->rom_name = strdup(filename);
    rom->rom_size = data_size;

    TraceLog(LOG_INFO, "CHIP-8 emulator initialized");
    return true;
}

void audio_init(chip8_t *chip8)
{
    InitAudioDevice();
    const unsigned int sample_rate = 44100;
    const unsigned int seconds = 1;

    Wave wave = {0};

    wave.frameCount = sample_rate * seconds;
    wave.sampleRate = sample_rate;
    wave.sampleSize = 16;
    wave.channels = 1;

    wave.data = malloc(wave.frameCount * sizeof(short));
    short *samples = (short *)wave.data;

    const float freq = 440.0f;
    const float period = sample_rate / freq;

    for (unsigned int i = 0; i < wave.frameCount; ++i)
    {
        samples[i] = (i % (int)period) < period / 2 ? SHRT_MAX : SHRT_MIN;
    }

    chip8->beep = LoadSoundFromWave(wave);
    UnloadWave(wave);
    chip8->sound_playing = false;
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
        TraceLog(LOG_INFO, "ESCAPE pressed, quitting");
        chip8->state = QUIT;
    }

    if (IsKeyPressed(KEY_P))
    {
        chip8->state = PAUSED;
    }

    if (IsKeyPressed(KEY_R))
    {
        chip8->state = RUNNING;
    }

    // CHIP-8 keypad layout
    // 1 2 3 C
    if (IsKeyDown(KEY_ONE))
        chip8->keypad[0x1] = 1;

    if (IsKeyDown(KEY_TWO))
        chip8->keypad[0x2] = 1;

    if (IsKeyDown(KEY_THREE))
        chip8->keypad[0x3] = 1;

    if (IsKeyDown(KEY_FOUR))
        chip8->keypad[0xC] = 1;

    // 4 5 6 D
    if (IsKeyDown(KEY_Q))
        chip8->keypad[0x4] = 1;

    if (IsKeyDown(KEY_W))
        chip8->keypad[0x5] = 1;

    if (IsKeyDown(KEY_E))
        chip8->keypad[0x6] = 1;

    if (IsKeyDown(KEY_R))
        chip8->keypad[0xD] = 1;

    // 7 8 9 E
    if (IsKeyDown(KEY_A))
        chip8->keypad[0x7] = 1;

    if (IsKeyDown(KEY_S))
        chip8->keypad[0x8] = 1;

    if (IsKeyDown(KEY_D))
        chip8->keypad[0x9] = 1;

    if (IsKeyDown(KEY_F))
        chip8->keypad[0xE] = 1;

    // A 0 B F
    if (IsKeyDown(KEY_Z))
        chip8->keypad[0xA] = 1;

    if (IsKeyDown(KEY_X))
        chip8->keypad[0x0] = 1;

    if (IsKeyDown(KEY_C))
        chip8->keypad[0xB] = 1;

    if (IsKeyDown(KEY_V))
        chip8->keypad[0xF] = 1;

    // CHIP-8 keypad layout
    // 1 2 3 C
    if (IsKeyUp(KEY_ONE))
        chip8->keypad[0x1] = 0;

    if (IsKeyUp(KEY_TWO))
        chip8->keypad[0x2] = 0;

    if (IsKeyUp(KEY_THREE))
        chip8->keypad[0x3] = 0;

    if (IsKeyUp(KEY_FOUR))
        chip8->keypad[0xC] = 0;

    // 4 5 6 D
    if (IsKeyUp(KEY_Q))
        chip8->keypad[0x4] = 0;

    if (IsKeyUp(KEY_W))
        chip8->keypad[0x5] = 0;

    if (IsKeyUp(KEY_E))
        chip8->keypad[0x6] = 0;

    if (IsKeyUp(KEY_R))
        chip8->keypad[0xD] = 0;

    // 7 8 9 E
    if (IsKeyUp(KEY_A))
        chip8->keypad[0x7] = 0;

    if (IsKeyUp(KEY_S))
        chip8->keypad[0x8] = 0;

    if (IsKeyUp(KEY_D))
        chip8->keypad[0x9] = 0;

    if (IsKeyUp(KEY_F))
        chip8->keypad[0xE] = 0;

    // A 0 B F
    if (IsKeyUp(KEY_Z))
        chip8->keypad[0xA] = 0;

    if (IsKeyUp(KEY_X))
        chip8->keypad[0x0] = 0;

    if (IsKeyUp(KEY_C))
        chip8->keypad[0xB] = 0;

    if (IsKeyUp(KEY_V))
        chip8->keypad[0xF] = 0;
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
            memset(chip8->video, 0, sizeof(chip8->video));
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
        // Skip next instruction if Vx = NN
        if (chip8->registers[chip8->instruction.X] == chip8->instruction.NN)
        {
            chip8->pc += 2;
        }
        break;
    case 0x04:
        if (chip8->registers[chip8->instruction.X] != chip8->instruction.NN)
        {
            chip8->pc += 2;
        }
        break;
    case 0x05:
        if (chip8->registers[chip8->instruction.X] == chip8->registers[chip8->instruction.Y])
        {
            chip8->pc += 2;
        }
        break;
    case 0x06:
        chip8->registers[chip8->instruction.X] = chip8->instruction.NN;
        break;
    case 0x07:
        chip8->registers[chip8->instruction.X] += chip8->instruction.NN;
        break;
    case 0x08:
        switch (chip8->instruction.N)
        {
        case 0x00:
            chip8->registers[chip8->instruction.X] = chip8->registers[chip8->instruction.Y];
            break;
        case 0x01:
            chip8->registers[chip8->instruction.X] |= chip8->registers[chip8->instruction.Y];
            break;
        case 0x02:
            chip8->registers[chip8->instruction.X] &= chip8->registers[chip8->instruction.Y];
            break;
        case 0x03:
            chip8->registers[chip8->instruction.X] ^= chip8->registers[chip8->instruction.Y];
            break;
        case 0x04:
        {
            uint16_t result = chip8->registers[chip8->instruction.X] + chip8->registers[chip8->instruction.Y];
            chip8->registers[0xF] = result > 0xFF;
            chip8->registers[chip8->instruction.X] = result & 0xFF;
        }
        break;
        case 0x05:
            chip8->registers[0xF] = chip8->registers[chip8->instruction.X] > chip8->registers[chip8->instruction.Y];
            chip8->registers[chip8->instruction.X] -= chip8->registers[chip8->instruction.Y];
            break;
        case 0x06:
            chip8->registers[0xF] = chip8->registers[chip8->instruction.X] & 0x1;
            chip8->registers[chip8->instruction.X] >>= 1;
            break;
        case 0x07:
            chip8->registers[0xF] = chip8->registers[chip8->instruction.Y] > chip8->registers[chip8->instruction.X];
            chip8->registers[chip8->instruction.X] = chip8->registers[chip8->instruction.Y] - chip8->registers[chip8->instruction.X];
            break;
        case 0x0E:
            chip8->registers[0xF] = (chip8->registers[chip8->instruction.X] >> 7) & 1;
            chip8->registers[chip8->instruction.X] <<= 1;
            break;
        default:
            break;
        }
        break;
    case 0x09:
        if (chip8->registers[chip8->instruction.X] != chip8->registers[chip8->instruction.Y])
        {
            chip8->pc += 2;
        }
        break;
    case 0x0A:
        chip8->I = chip8->instruction.NNN;
        break;
    case 0x0B:
        chip8->pc = chip8->registers[0] + chip8->instruction.NNN;
        break;
    case 0x0C:
        chip8->registers[chip8->instruction.X] = GetRandomValue(0, 255) & chip8->instruction.NN;
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
                    if (chip8->video[display_pixel_index])
                    {
                        chip8->registers[0xF] = 1;
                    }

                    chip8->video[display_pixel_index] ^= 1;
                }
            }
        }
        break;
    case 0x0E:
        if (chip8->instruction.NN == 0x9E)
        {
            if (chip8->keypad[chip8->registers[chip8->instruction.X]])
                chip8->pc += 2;
        }
        else if (chip8->instruction.NN == 0xA1)
        {
            if (!chip8->keypad[chip8->registers[chip8->instruction.X]])
                chip8->pc += 2;
        }
        break;
    case 0x0F:
        switch (chip8->instruction.NN)
        {
        case 0x0A:
            bool key_pressed = false;
            for (int i = 0; i < sizeof chip8->keypad; ++i)
            {
                if (chip8->keypad[i])
                {
                    chip8->registers[chip8->instruction.X] = i;
                    key_pressed = true;
                    break;
                }
            }

            if (key_pressed)
                chip8->pc += 2;
            break;
        case 0x1E:
            chip8->I += chip8->registers[chip8->instruction.X];
            break;
        case 0x07:
            chip8->registers[chip8->instruction.X] = chip8->delay_timer;
            break;
        case 0x15:
            chip8->delay_timer = chip8->registers[chip8->instruction.X];
            break;
        case 0x18:
            chip8->sound_timer = chip8->registers[chip8->instruction.X];
            break;
        case 0x29:
            chip8->I = chip8->registers[chip8->instruction.X] * 5;
            break;
        case 0x33:
            chip8->memory[chip8->I] = chip8->registers[chip8->instruction.X] / 100;
            chip8->memory[chip8->I + 1] = (chip8->registers[chip8->instruction.X] / 10) % 10;
            chip8->memory[chip8->I + 2] = chip8->registers[chip8->instruction.X] % 10;
            break;
        case 0x55:
            for (int i = 0; i <= chip8->instruction.X; ++i)
            {
                chip8->memory[chip8->I + i] = chip8->registers[i];
            }
            break;
        case 0x65:
            for (int i = 0; i <= chip8->instruction.X; ++i)
            {
                chip8->registers[i] = chip8->memory[chip8->I + i];
            }
            break;
        default:
            TraceLog(LOG_WARNING, "Unknown opcode: 0x0F%X", chip8->instruction.opcode);
            break;
        }
        break;
    default:
        TraceLog(LOG_WARNING, "Unknown opcode: 0x%X", chip8->instruction.opcode);
        break;
    };
}

void update_timers(chip8_t *chip8)
{
    if (chip8->delay_timer > 0)
    {
        chip8->delay_timer--;
    }

    if (chip8->sound_timer > 0)
    {
        if (!chip8->sound_playing)
        {
            PlaySound(chip8->beep);
            chip8->sound_playing = true;
        }

        chip8->sound_timer--;
    }
    else if (chip8->sound_playing)
    {
        StopSound(chip8->beep);
        chip8->sound_playing = false;
    }
}