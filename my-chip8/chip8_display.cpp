#include <stdio.h>
#include "chip8_display.h"

Chip8_Display::Chip8_Display(chip8_display_settings settings) : chip8_pixels_as_SDL_rectangles{},
                                                                chip8_pixels_as_horizontal_line_bits{0xffaa00ff}
{
    SDL_pixels_per_chip8_pixel = settings.SDL_pixels_per_chip8_pixel;
    chip8_pixel_border_in_SDL_pixels = settings.chip8_pixel_border_in_SDL_pixels;
    chip8_pixel_width_in_SDL_pixels = SDL_pixels_per_chip8_pixel - (2 * chip8_pixel_border_in_SDL_pixels);
    SDL_window_width = CHIP8_DISPLAY_WIDTH * SDL_pixels_per_chip8_pixel;
    SDL_window_height = CHIP8_DISPLAY_HEIGHT * SDL_pixels_per_chip8_pixel;

    window = SDL_CreateWindow("MyChip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_window_width, SDL_window_height, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    drawing_surface = SDL_GetWindowSurface(window);
    SDL_PixelFormat *pixel_format = drawing_surface->format;
    off_pixel_color = SDL_MapRGB(pixel_format, settings.off_pixel_red_value, settings.off_pixel_green_value, settings.off_pixel_blue_value);
    on_pixel_color = SDL_MapRGB(pixel_format, settings.on_pixel_red_value, settings.on_pixel_green_value, settings.on_pixel_blue_value);

    for (int i = 0; i < CHIP8_DISPLAY_HEIGHT; i++)
    {
        for (int j = 0; j < CHIP8_DISPLAY_WIDTH; j++)
        {
            chip8_pixels_as_SDL_rectangles[i][j].x = j * SDL_pixels_per_chip8_pixel + chip8_pixel_border_in_SDL_pixels;
            chip8_pixels_as_SDL_rectangles[i][j].y = i * SDL_pixels_per_chip8_pixel + chip8_pixel_border_in_SDL_pixels;
            chip8_pixels_as_SDL_rectangles[i][j].w = chip8_pixel_width_in_SDL_pixels;
            chip8_pixels_as_SDL_rectangles[i][j].h = chip8_pixel_width_in_SDL_pixels;
        }
    }
}

// void update_SDL_window(SDL_Surface *drawing_surface, std::bitset<CHIP8_DISPLAY_WIDTH> display_horizontal_lines[CHIP8_DISPLAY_HEIGHT])
void Chip8_Display::update()
{
    for (int i = 0; i < CHIP8_DISPLAY_HEIGHT; i++)
    {
        uint64_t current_horizontal_line_bitset = chip8_pixels_as_horizontal_line_bits[i];
        for (int j = 0; j < CHIP8_DISPLAY_WIDTH; j++)
        {
            int amount_to_shift = CHIP8_DISPLAY_WIDTH - (j + 1);
            bool current_pixel_value = (current_horizontal_line_bitset >> amount_to_shift) & 1;
            if (current_pixel_value)
                update_pixel(i, j, current_pixel_value);
        }
    }
    SDL_UpdateWindowSurface(window);
}

void Chip8_Display::update_pixel(int row, int column, bool pixel_value)
{
    uint32_t color;
    if (pixel_value == 0)
    {
        color = off_pixel_color;
    }
    else
    {
        color = on_pixel_color;
    }
    SDL_FillRect(drawing_surface, &chip8_pixels_as_SDL_rectangles[row][column], color);
}

void Chip8_Display::close()
{
    SDL_DestroyWindow(window);
}
