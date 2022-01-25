#include <fstream>
#include <filesystem>
#include <stdio.h>

#include "chip8_device.h"

namespace fs = std::filesystem;

Chip8_Device::Chip8_Device(Chip8_Display display) : memory{
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
                                                    },
                                                    PC{512}, index_register{0}, delay_timer{0}, sound_timer{0}, register_file{0}, display{display}
{
}

void Chip8_Device::open_ROM(const char *rom_string_path)
{
    fs::path rom_filesystem_path = rom_string_path;
    uintmax_t rom_size_in_bytes = fs::file_size(rom_filesystem_path);
    if (rom_size_in_bytes > MAX_ROM_SIZE_IN_BYTES)
    {
        printf("ROM file too large\n");
        exit(1);
    }

    FILE *rom_file = NULL;
    fopen_s(&rom_file, rom_string_path, "rb");
    if (rom_file == 0)
    {
        printf("Error opening ROM\n");
        exit(1);
    }
    size_t bytes_read = fread(&memory[512], sizeof(char), rom_size_in_bytes, rom_file);

    printf("rom bytes read = %zu\n", bytes_read);
}

void Chip8_Device::read_instruction_at_PC()
{
    uint16_t current_instruction_in_bits = (memory[PC] << 8) | memory[PC + 1];
    current_instruction.set(current_instruction_in_bits);
}

void Chip8_Device::increment_PC()
{
    PC += MEMORY_ADDRESSES_PER_INSTRUCTION;
}

uint8_t Chip8_Device::get_opcode()
{
    return current_instruction.first_nibble;
}

void Chip8_Device::clear_display()
{
    display.clear();
    display.draw_to_window();
}
void Chip8_Device::set_PC_to_immediate_value()
{
    PC = current_instruction.last_12_bits;
}
void Chip8_Device::set_register_to_immediate_value()
{
    uint8_t register_num = current_instruction.second_nibble;
    uint8_t new_register_value = current_instruction.second_byte;
    register_file[register_num] = new_register_value;
}
void Chip8_Device::add_immediate_value_to_register()
{
    uint8_t register_num = current_instruction.second_nibble;
    uint8_t num_to_add_to_register = current_instruction.second_byte;
    register_file[register_num] += num_to_add_to_register;
}
void Chip8_Device::set_index_register_to_immediate_value()
{
    index_register = current_instruction.last_12_bits;
}
void Chip8_Device::draw_sprite_at_index_address_to_coordinates_in_registers()
{
    uint8_t x_coordinate_register_num = current_instruction.second_nibble;
    uint8_t y_coordinate_register_num = current_instruction.third_nibble;
    uint8_t x_coordinate = register_file[x_coordinate_register_num];
    uint8_t y_coordinate = register_file[y_coordinate_register_num];
    uint8_t sprite_height = current_instruction.fourth_nibble;
    uint8_t *sprite_pixels = &memory[index_register];
    display.update_pixels_using_sprite(sprite_pixels, sprite_height, x_coordinate, y_coordinate);
    display.draw_to_window();
}

void Chip8_Device::print_device_state_info()
{
    printf("PC: %d\n", PC);
    printf("instruction: %04x\n", current_instruction.all_bits);
    printf("index register: %4d\n", index_register);
    for (int i = 0; i < NUMBER_OF_REGISTERS; i++)
    {
        printf("V%d: 0x%02x\n", i, register_file[i]);
    }
    int memory_step = 4;
    for (int i = 512; i < 700; i += memory_step)
    {
        printf("%d-%d: %02x %02x %02x %02x\n", i, i + memory_step, memory[i], memory[i + 1], memory[i + 2], memory[i + 3]);
    }
    display.print_pixels_to_console();
}

void Instruction::set(uint16_t instruction_in_bits)
{
    all_bits = instruction_in_bits;
    first_nibble = instruction_in_bits >> 12;
    second_nibble = (instruction_in_bits >> 8) & 0xf;
    third_nibble = (instruction_in_bits >> 4) & 0xf;
    fourth_nibble = instruction_in_bits & 0xf;
    second_byte = instruction_in_bits & 0xff;
    last_12_bits = instruction_in_bits & 0xfff;
}