#pragma once

#include <stack>
#include "chip8_display.h"
#include "chip8_keypad.h"
#include "instruction.h"

class Chip8_Device
{

public:
    Chip8_Device(Chip8_Display);
    void open_ROM(const char *);
    void cpu_tick();
    void print_device_state_info();

private:
    static const int MEMORY_SIZE_IN_BYTES = 4096;
    static const int NUMBER_OF_REGISTERS = 16;
    // static const int BITS_PER_MEMORY_ADDRESS = 8;
    static const int MEMORY_ADDRESSES_PER_INSTRUCTION = 2;
    static const int MAX_ROM_SIZE_IN_BYTES = 4096 - 512;
    static const int FONT_SPRITE_HEIGHT = 5;

    uint8_t memory[MEMORY_SIZE_IN_BYTES] = {
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
    uint16_t PC;
    uint16_t index_register;
    std::stack<uint16_t> stack;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t register_file[NUMBER_OF_REGISTERS];
    Chip8_Display display;
    Chip8_Keypad keypad;
    Instruction current_instruction;
    int clock_cycles_since_delay_timer_decremented;
    int clock_cycles_since_sound_timer_decremented;

    void read_instruction_at_PC();
    void increment_PC();
    void decrement_PC();
    void update_keypad_state();
    void perform_instruction();
    void handle_timers();
    void handle_timer(uint8_t *, int *);
    uint8_t get_register_X_value();
    uint8_t get_register_Y_value();

    /* All below functions found in chip8_device_operations.cpp */

    /* Instruction functions begin */

    void clear_display();
    void return_from_subroutine();
    void set_PC_to_immediate_value();
    void enter_subroutine_at_immediate_value();
    void skip_instruction_if_register_equals_immediate();
    void skip_instruction_if_register_not_equal_to_immediate();
    void skip_instruction_if_registers_equal();
    void skip_instruction_if_registers_not_equal();
    void set_register_to_immediate_value();
    void add_immediate_value_to_register();
    void set_register_X_to_register_Y_value();
    void OR_registers();
    void AND_registers();
    void XOR_registers();
    void add_registers();
    void subtract_register_Y_from_register_X();
    void subtract_register_X_from_register_Y();
    void right_shift_register();
    void left_shift_register();
    void set_index_register_to_immediate_value();
    void set_PC_to_immediate_value_plus_register_value();
    void set_register_to_random_number_ANDed_with_immediate_value();
    void draw_sprite_at_index_address_to_coordinates_in_registers();
    void skip_instruction_if_key_pressed();
    void skip_instruction_if_key_not_pressed();
    void set_register_to_delay_timer_value();
    void set_delay_timer_to_register_value();
    void set_sound_timer_to_register_value();
    void add_register_value_to_index_register();
    void wait_for_keypress();
    void set_register_to_pressed_key();
    void set_index_register_to_address_of_font_specified_in_register();
    void store_register_value_digits_in_memory();
    void store_register_values_in_memory();
    void load_memory_values_into_registers();

    /* Instruction functions end */

    /* Instruction helpers begin */

    void skip_instruction_if(bool);
    bool register_equals_immediate_value();
    bool registers_equal();

    bool key_specified_in_register_is_pressed();

    chip8_sprite get_sprite_from_memory();
    display_coordinates get_sprite_coordinates_from_registers();
    void set_flag_indicating_if_any_pixels_turned_off();

    uint16_t get_font_memory_address(uint8_t);

    /* Instruction helpers end */
};
