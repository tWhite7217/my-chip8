#include <SDL.h>
#include <stdio.h>
#include <iterator>
#include <algorithm>
#include <bitset>

#include "main.h"
#include "chip8_display.h"
#include "chip8_device.h"

int main(int argc, char *args[])
{
	if (SDL_InitSubSystem(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	chip8_display_settings display_settings = {
		.SDL_pixels_per_chip8_pixel = 14,
		.chip8_pixel_border_in_SDL_pixels = 1,
		.off_pixel_red_value = 0x00,
		.off_pixel_green_value = 0x00,
		.off_pixel_blue_value = 0x00,
		.on_pixel_red_value = 0xff,
		.on_pixel_green_value = 0xff,
		.on_pixel_blue_value = 0xff};
	Chip8_Display display(display_settings);

	Chip8_Device device(display);

	const char *rom_string_path = "D:\\Downloads\\PONG(1P).ch8";
	device.open_ROM(rom_string_path);

	while (true)
	{
		device.cpu_tick();
		// device.print_device_state_info();
	}

	SDL_Quit();

	return 0;
}
