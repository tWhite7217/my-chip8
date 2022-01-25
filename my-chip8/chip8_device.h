#pragma once

#include <stack>
#include "chip8_display.h"

class Instruction
{
public:
    void set(uint16_t instruction_in_bits);
    uint16_t all_bits;
    uint8_t first_nibble;
    uint8_t second_nibble;
    uint8_t third_nibble;
    uint8_t fourth_nibble;
    uint8_t second_byte;
    uint16_t last_12_bits;
};

class Chip8_Device
{
private:
    static const int MEMORY_SIZE_IN_BYTES = 4096;
    static const int NUMBER_OF_REGISTERS = 16;
    // static const int BITS_PER_MEMORY_ADDRESS = 8;
    static const int MEMORY_ADDRESSES_PER_INSTRUCTION = 2;
    static const int MAX_ROM_SIZE_IN_BYTES = 4096 - 512;
    uint8_t memory[MEMORY_SIZE_IN_BYTES];
    uint16_t PC;
    uint16_t index_register;
    std::stack<uint16_t> stack;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t register_file[NUMBER_OF_REGISTERS];
    Chip8_Display display;
    Instruction current_instruction;

public:
    Chip8_Device(Chip8_Display display);
    void open_ROM(const char *rom_string_path);
    void read_instruction_at_PC();
    void increment_PC();
    uint8_t get_opcode();
    void clear_display();
    void set_PC_to_immediate_value();
    void set_register_to_immediate_value();
    void add_immediate_value_to_register();
    void set_index_register_to_immediate_value();
    void draw_sprite_at_index_address_to_coordinates_in_registers();
    void print_device_state_info();
};