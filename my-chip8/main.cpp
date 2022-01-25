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
	chip8_display_settings display_settings = {
		.SDL_pixels_per_chip8_pixel = 10,
		.chip8_pixel_border_in_SDL_pixels = 2,
		.off_pixel_red_value = 0x00,
		.off_pixel_green_value = 0x00,
		.off_pixel_blue_value = 0x00,
		.on_pixel_red_value = 0xff,
		.on_pixel_green_value = 0xff,
		.on_pixel_blue_value = 0xff};
	Chip8_Display display(display_settings);

	Chip8_Device device(display);

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	const char *rom_string_path = "D:\\Downloads\\IBM Logo.ch8";
	device.open_ROM(rom_string_path);

	while (true)
	{
		device.read_instruction_at_PC();
		device.increment_PC();

		switch (device.get_opcode())
		{
		case 0x0:
			device.clear_display();
			break;

		case 0x1:
			device.set_PC_to_immediate_value();
			break;

		case 0x6:
			device.set_register_to_immediate_value();
			break;

		case 0x7:
			device.add_immediate_value_to_register();
			break;

		case 0xA:
			device.set_index_register_to_immediate_value();
			break;

		case 0xD:
			device.draw_sprite_at_index_address_to_coordinates_in_registers();
			break;

		default:
			printf("Error: unknown opcode.");
			exit(1);
			break;
		}

		SDL_Delay(50);
		// device.print_device_state_info();
	}

	SDL_Quit();

	return 0;
}
