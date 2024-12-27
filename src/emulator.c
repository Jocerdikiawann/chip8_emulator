#include "emulator.h"

void chip8_init(chip8_t *chip8, const char *filename)
{
    const uint8_t entry_point = 0x200; // CHIP-8 programs start at 0x200
    unsigned int datasize = 0;
    unsigned char file = LoadFileData(filename, &datasize);
    if (file != NULL)
    {
        memcpy(chip8->memory[entry_point], file, datasize);
        UnloadFileData(file);
    }
}