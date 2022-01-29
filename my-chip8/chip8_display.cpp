#include <stdio.h>
#include <string.h>
#include "chip8_display.h"

Chip8_Display::Chip8_Display(chip8_display_settings settings) : pixels_as_SDL_rectangles{},
                                                                horizontal_display_lines{}
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
            pixels_as_SDL_rectangles[i][j].x = j * SDL_pixels_per_chip8_pixel +
                                               chip8_pixel_border_in_SDL_pixels;
            pixels_as_SDL_rectangles[i][j].y = i * SDL_pixels_per_chip8_pixel +
                                               chip8_pixel_border_in_SDL_pixels;
            pixels_as_SDL_rectangles[i][j].w = chip8_pixel_width_in_SDL_pixels;
            pixels_as_SDL_rectangles[i][j].h = chip8_pixel_width_in_SDL_pixels;
        }
    }
}

// void update_SDL_window(SDL_Surface *drawing_surface, std::bitset<CHIP8_DISPLAY_WIDTH> display_horizontal_lines[CHIP8_DISPLAY_HEIGHT])
void Chip8_Display::update_display_window()
{
    for (int i = 0; i < CHIP8_DISPLAY_HEIGHT; i++)
    {
        uint64_t current_horizontal_line_bitset = horizontal_display_lines[i];
        for (int j = 0; j < CHIP8_DISPLAY_WIDTH; j++)
        {
            int amount_to_shift = CHIP8_DISPLAY_WIDTH - (j + 1);
            bool new_pixel_value = (current_horizontal_line_bitset >> amount_to_shift) & 1;
            draw_one_pixel(i, j, new_pixel_value);
        }
    }
    SDL_UpdateWindowSurface(window);
}

void Chip8_Display::draw_one_pixel(int row, int column, bool pixel_value)
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
    SDL_FillRect(drawing_surface, &pixels_as_SDL_rectangles[row][column], color);
}

void Chip8_Display::draw_sprite(chip8_sprite sprite, display_coordinates coordinates)
{
    coordinates.x = coordinates.x % CHIP8_DISPLAY_WIDTH;
    int amount_to_shift = CHIP8_DISPLAY_WIDTH - (coordinates.x + SPRITE_WIDTH);
    at_least_one_pixel_turned_off_when_last_sprite_was_drawn = false;

    for (int i = 0; i < sprite.height && coordinates.y + i < CHIP8_DISPLAY_HEIGHT; i++)
    {
        uint64_t *display_line_to_update =
            &horizontal_display_lines[coordinates.y + i];
        uint64_t sprite_line_shifted_into_place =
            shift_sprite_line_left(sprite.horizontal_lines[i], amount_to_shift);

        if (drawing_sprite_line_will_turn_off_pixel(display_line_to_update, sprite_line_shifted_into_place))
        {
            at_least_one_pixel_turned_off_when_last_sprite_was_drawn = true;
        }

        update_line_using_shifted_sprite_line(display_line_to_update, sprite_line_shifted_into_place);
    }
}

uint64_t Chip8_Display::shift_sprite_line_left(uint64_t sprite_line, int amount_to_shift)
{
    if (amount_to_shift < 0)
    {
        amount_to_shift = abs(amount_to_shift);
        return sprite_line >> amount_to_shift;
    }
    else
    {
        return sprite_line << amount_to_shift;
    }
}

bool Chip8_Display::drawing_sprite_line_will_turn_off_pixel(uint64_t *display_line_to_update, uint64_t sprite_line_shifted_into_place)
{
    return *display_line_to_update & sprite_line_shifted_into_place;
}

void Chip8_Display::update_line_using_shifted_sprite_line(uint64_t *display_line_to_update, uint64_t sprite_line_shifted_into_place)
{
    *display_line_to_update ^= sprite_line_shifted_into_place;
}

void Chip8_Display::clear()
{
    memset(horizontal_display_lines, 0, sizeof(horizontal_display_lines));
}

void Chip8_Display::close()
{
    SDL_DestroyWindow(window);
}

void Chip8_Display::print_display_to_console()
{
    for (int i = 0; i < CHIP8_DISPLAY_HEIGHT; i++)
    {
        uint64_t current_horizontal_line_bitset = horizontal_display_lines[i];
        for (int j = 0; j < CHIP8_DISPLAY_WIDTH; j++)
        {
            int amount_to_shift = CHIP8_DISPLAY_WIDTH - (j + 1);
            bool current_pixel_value = (current_horizontal_line_bitset >> amount_to_shift) & 1;
            if (current_pixel_value)
            {
                printf("#");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}