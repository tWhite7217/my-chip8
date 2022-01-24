#include <SDL.h>
#include <stdio.h>
#include <stack>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <filesystem>
#include <bitset>

namespace fs = std::filesystem;

#include "main.h"
#include "chip8_display.h"

const int MAX_ROM_SIZE_IN_BYTES = 4096 - 512;

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

	uint8_t memory[4096] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	uint16_t PC = 512;
	uint16_t index_register = 0;
	std::stack<uint16_t> stack;
	uint8_t delay_timer = 0;
	uint8_t sound_timer = 0;
	uint8_t reg_file[16] = {};

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	display.update();

	SDL_Delay(2000);

	// const char *rom_string_path = "D:\\Downloads\\IBM Logo.ch8";
	// fs::path rom_filesystem_path = rom_string_path;
	// uintmax_t rom_size_in_bytes = fs::file_size(rom_filesystem_path);
	// if (rom_size_in_bytes > MAX_ROM_SIZE_IN_BYTES)
	// {
	// 	exit(1);
	// }

	// FILE *rom_file = NULL;
	// fopen_s(&rom_file, rom_string_path, "rb");
	// if (rom_file == 0)
	// {
	// 	printf("Error opening ROM\n");
	// 	exit(1);
	// }
	// int bytes_read = fread(&memory[512], sizeof(char), rom_size_in_bytes, rom_file);

	// printf("rom bytes read = %d\n", bytes_read);

	// unsigned short current_instruction;
	// unsigned char first_nibble;

	// // while (TRUE)
	// // {
	// // printf("char at PC %d: %x\n", PC, memory[PC]);
	// // printf("char at PC %d: %x\n", PC + 1, memory[PC + 1]);

	// current_instruction = (memory[PC] << 8) | memory[PC + 1];
	// printf("instruction at PC %d: %04x\n", PC, current_instruction);
	// PC += 2;

	// first_nibble = current_instruction >> 12;

	// switch (first_nibble)
	// {
	// case 0:
	// 	SDL_FillRect(drawing_surface, NULL, SDL_MapRGB(drawing_surface->format, 0x00, 0x00, 0x00));

	// 	SDL_UpdateWindowSurface(window);
	// 	SDL_Delay(2000);
	// 	break;

	// default:
	// 	printf("Error: unknown opcode.");
	// 	exit(1);
	// 	break;
	// }
	// // }

	SDL_Quit();

	return 0;
}
