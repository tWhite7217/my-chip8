#include <stdio.h>
#include <string.h>
#include "chip8_display.h"

Chip8_Display::Chip8_Display(chip8_display_settings settings) : pixels_as_SDL_rectangles{},
                                                                horizontal_display_lines{},
                                                                pixel_colors_as_array_index{}
{
    SDL_pixels_per_chip8_pixel = settings.SDL_pixels_per_chip8_pixel;
    chip8_pixel_border_in_SDL_pixels = settings.chip8_pixel_border_in_SDL_pixels;
    chip8_pixel_width_in_SDL_pixels = SDL_pixels_per_chip8_pixel - (2 * chip8_pixel_border_in_SDL_pixels);
    SDL_window_width = CHIP8_DISPLAY_WIDTH * SDL_pixels_per_chip8_pixel;
    SDL_window_height = CHIP8_DISPLAY_HEIGHT * SDL_pixels_per_chip8_pixel;

    num_frames_off_pixel_fades = settings.num_frames_off_pixel_fades;
    fade_step_fraction = ((float)1) / (num_frames_off_pixel_fades + 1);
    pixel_color_array_size = num_frames_off_pixel_fades + 2;

    window = SDL_CreateWindow("MyChip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_window_width, SDL_window_height, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    drawing_surface = SDL_GetWindowSurface(window);

    possible_pixel_colors_from_fully_off_to_fully_on = (uint32_t *)malloc(pixel_color_array_size * sizeof(uint32_t));
    generate_display_colors(settings);

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

void Chip8_Display::generate_display_colors(chip8_display_settings settings)
{
    SDL_PixelFormat *pixel_format = drawing_surface->format;

    for (int i = 0; i < pixel_color_array_size; i++)
    {
        float on_color_fraction = i * fade_step_fraction;
        uint8_t blended_red_value = blend_color_channel_value(settings.on_pixel_red_value, settings.off_pixel_red_value, on_color_fraction);
        uint8_t blended_green_value = blend_color_channel_value(settings.on_pixel_green_value, settings.off_pixel_green_value, on_color_fraction);
        uint8_t blended_blue_value = blend_color_channel_value(settings.on_pixel_blue_value, settings.off_pixel_blue_value, on_color_fraction);
        possible_pixel_colors_from_fully_off_to_fully_on[i] = SDL_MapRGB(pixel_format, blended_red_value, blended_green_value, blended_blue_value);
    }
}

uint8_t Chip8_Display::blend_color_channel_value(uint8_t value_1, uint8_t value_2, float fraction_of_value_1)
{
    float fraction_of_value_2 = 1 - fraction_of_value_1;
    return (value_1 * fraction_of_value_1) + (value_2 * fraction_of_value_2);
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
            bool pixel_value_is_on = (current_horizontal_line_bitset >> amount_to_shift) & 1;
            draw_one_pixel(i, j, pixel_value_is_on);
        }
    }
    SDL_UpdateWindowSurface(window);
}

void Chip8_Display::draw_one_pixel(int row, int column, bool pixel_is_on)
{
    size_t *pixel_color_index = &pixel_colors_as_array_index[row][column];
    if (pixel_is_on)
    {
        *pixel_color_index = pixel_color_array_size - 1;
    }
    else
    {
        if (*pixel_color_index > 0)
        {
            *pixel_color_index -= 1;
        }
    }
    uint32_t color = possible_pixel_colors_from_fully_off_to_fully_on[*pixel_color_index];
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
    free(possible_pixel_colors_from_fully_off_to_fully_on);
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