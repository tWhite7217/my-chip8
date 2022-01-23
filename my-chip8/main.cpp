#include <SDL.h>
#include <stdio.h>
#include <stack>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

#include "main.h"

#define TRUE 1
#define FALSE 1

std::ifstream::pos_type filesize(const char *filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

int main(int argc, char *args[])
{
	SDL_Window *window = NULL;
	SDL_Surface *drawing_surface = NULL;

	char memory[4096] = {
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
	short PC = 512;
	short index_register = 0;
	std::stack<short> stack;
	char delay_timer = 0;
	char sound_timer = 0;
	char reg_file[16] = {0};

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			// drawing_surface = SDL_GetWindowSurface(window);

			// SDL_FillRect(drawing_surface, NULL, SDL_MapRGB(drawing_surface->format, 0xFF, 0xFF, 0xFF));

			// SDL_UpdateWindowSurface(window);

			// SDL_Delay(2000);
		}
	}

	const char *rom_string_path = "D:\\Downloads\\IBM Logo.ch8";
	fs::path rom_filesystem_path = rom_string_path;
	uintmax_t rom_size_in_bytes = fs::file_size(rom_filesystem_path);

	FILE *rom_file = NULL;
	fopen_s(&rom_file, rom_string_path, "rb");
	if (rom_file == 0)
	{
		printf("Error opening ROM\n");
		exit(1);
	}
	int bytes_read = fread(&memory[512], sizeof(char), rom_size_in_bytes, rom_file);

	printf("rom bytes read = %d\n", bytes_read);

	// while (TRUE) {

	// }

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}