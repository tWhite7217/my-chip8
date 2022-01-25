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
    static const int SPRITE_WIDTH = 8;
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

    void draw_one_pixel(int row, int column, bool pixel_value);

public:
    Chip8_Display(chip8_display_settings settings);
    void draw_to_window();
    void update_pixels_using_sprite(uint8_t sprite_pixels[], uint8_t sprite_height, uint8_t x_coordinate, uint8_t y_coordinate);
    void clear();
    void close();
    void print_pixels_to_console();
};