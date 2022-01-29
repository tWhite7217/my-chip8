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

typedef struct
{
    uint8_t *horizontal_lines;
    uint8_t height;
} chip8_sprite;

typedef struct
{
    uint8_t x;
    uint8_t y;
} display_coordinates;

class Chip8_Display
{
public:
    bool at_least_one_pixel_turned_off_when_last_sprite_was_drawn;

    Chip8_Display(chip8_display_settings settings);
    void update_display_window();
    void draw_sprite(chip8_sprite sprite, display_coordinates coordinates);
    void clear();
    void close();
    void print_display_to_console();

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
    SDL_Rect pixels_as_SDL_rectangles[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
    uint64_t horizontal_display_lines[CHIP8_DISPLAY_HEIGHT];
    uint32_t off_pixel_color;
    uint32_t on_pixel_color;

    void draw_one_pixel(int row, int column, bool pixel_value);
    uint64_t shift_sprite_line_left(uint64_t sprite_line, int amount_to_shift);
    bool drawing_sprite_line_will_turn_off_pixel(uint64_t *display_line_to_update, uint64_t sprite_line_shifted_into_place);
    void update_line_using_shifted_sprite_line(uint64_t *display_line_to_update, uint64_t sprite_line_shifted_into_place);
};