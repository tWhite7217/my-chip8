#pragma once

#include <SDL.h>

typedef struct
{
    int SDL_pixels_per_chip8_pixel;
    int chip8_pixel_border_in_SDL_pixels;
    int num_frames_off_pixel_fades;
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

    Chip8_Display(chip8_display_settings);
    void update_display_window();
    void draw_sprite(chip8_sprite, display_coordinates);
    void clear();
    void close();
    void print_display_to_console();

private:
    static const int CHIP8_DISPLAY_WIDTH = 64;
    static const int CHIP8_DISPLAY_HEIGHT = 32;
    static const int SPRITE_WIDTH = 8;
    int num_frames_off_pixel_fades;
    float fade_step_fraction;
    int pixel_color_array_size;
    int SDL_pixels_per_chip8_pixel;
    int chip8_pixel_border_in_SDL_pixels;
    int chip8_pixel_width_in_SDL_pixels;
    int SDL_window_width;
    int SDL_window_height;

    SDL_Window *window;
    SDL_Surface *drawing_surface;
    SDL_Rect pixels_as_SDL_rectangles[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
    size_t pixel_colors_as_array_index[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
    uint64_t horizontal_display_lines[CHIP8_DISPLAY_HEIGHT];
    uint32_t *possible_pixel_colors_from_fully_off_to_fully_on;

    void generate_display_colors(chip8_display_settings);
    uint8_t blend_color_channel_value(uint8_t value_1, uint8_t value_2, float fraction_of_value_1);
    void draw_one_pixel(int, int, bool);
    uint64_t shift_sprite_line_left(uint64_t, int);
    bool drawing_sprite_line_will_turn_off_pixel(uint64_t *, uint64_t);
    void update_line_using_shifted_sprite_line(uint64_t *, uint64_t);
};