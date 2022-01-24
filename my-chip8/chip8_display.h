#pragma once

#include <SDL.h>

typedef struct
{
    int SDL_pixels_per_chip8_pixel;
    int chip8_pixel_border_in_SDL_pixels;
    uint8_t off_pixel_red_value;
    uint8_t off_pixel_green_value;
    uint8_t off_pixel_blue_value;
    uint8_t on_pixel_red_value;
    uint8_t on_pixel_green_value;
    uint8_t on_pixel_blue_value;
} chip8_display_settings;

class Chip8_Display
{
private:
    static const int CHIP8_DISPLAY_WIDTH = 64;
    static const int CHIP8_DISPLAY_HEIGHT = 32;
    int SDL_pixels_per_chip8_pixel;
    int chip8_pixel_border_in_SDL_pixels;
    int chip8_pixel_width_in_SDL_pixels;
    int SDL_window_width;
    int SDL_window_height;

    SDL_Window *window;
    SDL_Surface *drawing_surface;
    SDL_Rect chip8_pixels_as_SDL_rectangles[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
    uint64_t chip8_pixels_as_horizontal_line_bits[CHIP8_DISPLAY_HEIGHT];
    uint32_t off_pixel_color;
    uint32_t on_pixel_color;

    bool initialized_correctly;

    void update_pixel(int row, int column, bool pixel_value);

public:
    Chip8_Display(chip8_display_settings settings);
    void update();
    void close();
};