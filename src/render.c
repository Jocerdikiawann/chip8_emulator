#include "render.h"

void init_display()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    int full_width, full_height;

#ifdef PLATFORM_WEB
    full_height = 800;
    full_width = 400;
#else
    int factor = 50;
    full_width = factor * 16;
    full_height = factor * 9;
#endif
    InitWindow(full_width, full_height, "CHIP-8 Emulator");

    SetWindowMinSize(320, 160);
}

void render_main_menu(menu_t *menu)
{
    Vector2 pos_box = {
        .x = GetScreenWidth() * 0.5f,
        .y = GetScreenHeight() * 0.5f,
    };

    float box_width = GetScreenWidth() - (40 * 2);
    float box_height = GetScreenHeight() - (40 * 2);

    DrawRectangleLines(
        pos_box.x - (box_width / 2),
        pos_box.y - (box_height / 2),
        box_width,
        box_height,
        RAYWHITE);

    if (menu->count == 0)
    {
        const char *text = "Drag and drop ROMs here";
        Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
        DrawText(text, pos_box.x - (text_size.x / 2), pos_box.y - (text_size.y / 2), 20, RAYWHITE);
    }

    if (menu->count != 0)
    {
        for (size_t i = 0; i < menu->count; i++)
        {
            if (menu->items[i].roms_name == NULL)
            {
                TraceLog(LOG_INFO, "ROM name is NULL at index %d", i);
                continue;
            }
            printf("INDEX %d\n", i);
            printf("Rom name size: %d\n", TextLength(menu->items[i].roms_name));
            char *text = TextFormat("%d. %s", i + 1, menu->items[i].roms_name);
            Vector2 text_size = MeasureTextEx(GetFontDefault(), text, 20, 1);
            Vector2 text_pos = {
                .x = pos_box.x - text_size.x * 0.5f,
                .y = pos_box.y - box_height * 0.5f + 20 + (i * 40),
            };
            DrawText(text, text_pos.x, text_pos.y, 20, RAYWHITE);
        }
    }
}

void utility_drag_drop_rom(menu_t *menu)
{
    FilePathList dropped_files = LoadDroppedFiles();
    if (dropped_files.count <= 0)
    {
        TraceLog(LOG_INFO, "No files dropped");
        UnloadDroppedFiles(dropped_files);
        return;
    }

    if (dropped_files.count > 10)
    {
        TraceLog(LOG_WARNING, "Too many files dropped: %i", dropped_files.count);
        UnloadDroppedFiles(dropped_files);
        return;
    }

    for (size_t i = 0; i < dropped_files.count; ++i)
    {
        char *file_path = strdup(dropped_files.paths[i]);
        if (!IsFileExtension(file_path, ".ch8"))
        {
            TraceLog(LOG_WARNING, "Invalid file extension: %s", file_path);
            free(file_path);
            continue;
        }

        if (menu->count >= menu->capacity)
        {
            size_t new_size = sizeof(menu_item_t) * (menu->capacity + dropped_files.count);
            menu_item_t *new_items = realloc(menu->items, new_size);
            if (new_items == NULL)
            {
                TraceLog(LOG_ERROR, "Failed to reallocate, new buffer capacity: %i", new_size);
                free(file_path);
                return;
            }
            menu->items = new_items;
            menu->capacity += dropped_files.count;
        }

        TraceLog(LOG_INFO, "Current capacity: %d", menu->capacity);

        menu->items[menu->count].roms = file_path;

        const char *file_name = GetFileName(file_path);
        TraceLog(LOG_INFO, "Adding ROM %s at index %d", file_name, menu->count);
        if (TextLength(file_name) > 23)
        {
            char *copy_string = malloc(24);
            strncpy(copy_string, file_name, 24);
            copy_string[24] = '\0';
            menu->items[i].roms_name = copy_string;
        }
        else
        {
            menu->items[i].roms_name = file_name;
        }

        menu->count++;
    }
    UnloadDroppedFiles(dropped_files);
}

void render_display(chip8_t *chip8)
{
    int window_width = GetScreenWidth();
    int window_height = GetScreenHeight();

    float target_aspect = 2.0f; // 64/32
    float current_aspect = (float)window_width / window_height;

    int display_width, display_height;
    if (current_aspect > target_aspect)
    {
        display_height = window_height;
        display_width = window_height * target_aspect;
    }
    else
    {
        display_width = window_width;
        display_height = window_width / target_aspect;
    }

    int offset_x = (window_width - display_width) / 2;
    int offset_y = (window_height - display_height) / 2;

    float pixel_width = (float)display_width / CHIP8_WIDTH;
    float pixel_height = (float)display_height / CHIP8_HEIGHT;

    for (int y = 0; y < CHIP8_HEIGHT; ++y)
    {
        for (int x = 0; x < CHIP8_WIDTH; ++x)
        {
            if (chip8->video[y * CHIP8_WIDTH + x])
            {
                DrawRectangle(
                    offset_x + (x * pixel_width),
                    offset_y + (y * pixel_height),
                    ceil(pixel_width),
                    ceil(pixel_height),
                    WHITE);

                DrawRectangleLines(
                    offset_x + (x * pixel_width),
                    offset_y + (y * pixel_height),
                    ceil(pixel_width),
                    ceil(pixel_height),
                    BLACK);
            }
        }
    }
}