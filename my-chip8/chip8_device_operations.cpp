#include "chip8_device.h"

void Chip8_Device::clear_display()
{
    display.clear();
    display.update_display_window();
}

void Chip8_Device::return_from_subroutine()
{
    uint16_t old_PC = stack.top();
    stack.pop();
    PC = old_PC;
}

void Chip8_Device::set_PC_to_immediate_value()
{
    PC = current_instruction.get_12_bit_immediate();
}

void Chip8_Device::enter_subroutine_at_immediate_value()
{
    stack.push(PC);
    set_PC_to_immediate_value();
}

void Chip8_Device::skip_instruction_if_register_equals_immediate()
{
    skip_instruction_if(register_equals_immediate_value());
}

void Chip8_Device::skip_instruction_if_register_not_equal_to_immediate()
{
    skip_instruction_if(!register_equals_immediate_value());
}

void Chip8_Device::skip_instruction_if_registers_equal()
{
    skip_instruction_if(registers_equal());
}

void Chip8_Device::skip_instruction_if_registers_not_equal()
{
    skip_instruction_if(!registers_equal());
}

void Chip8_Device::skip_instruction_if(bool should_skip)
{
    if (should_skip)
    {
        increment_PC();
    }
}

bool Chip8_Device::register_equals_immediate_value()
{
    uint8_t register_value = get_register_X_value();
    uint8_t immediate_value = current_instruction.get_8_bit_immediate();
    return register_value == immediate_value;
}

bool Chip8_Device::registers_equal()
{
    uint8_t register_X_value = get_register_X_value();
    uint8_t register_Y_value = get_register_Y_value();
    return register_X_value == register_Y_value;
}

void Chip8_Device::set_register_to_immediate_value()
{
    uint8_t register_num = current_instruction.get_register_X_number();
    uint8_t new_register_value = current_instruction.get_8_bit_immediate();
    register_file[register_num] = new_register_value;
}

void Chip8_Device::add_immediate_value_to_register()
{
    uint8_t register_num = current_instruction.get_register_X_number();
    uint8_t num_to_add_to_register = current_instruction.get_8_bit_immediate();
    register_file[register_num] += num_to_add_to_register;
}

void Chip8_Device::set_register_X_to_register_Y_value()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    register_file[reg_X_num] = get_register_Y_value();
}

void Chip8_Device::OR_registers()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    register_file[reg_X_num] = get_register_X_value() | get_register_Y_value();
}

void Chip8_Device::AND_registers()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    register_file[reg_X_num] = get_register_X_value() & get_register_Y_value();
}

void Chip8_Device::XOR_registers()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    register_file[reg_X_num] = get_register_X_value() ^ get_register_Y_value();
}

void Chip8_Device::add_registers()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    uint16_t reg_X_value = get_register_X_value();
    uint16_t reg_Y_value = get_register_Y_value();
    uint16_t sum = reg_X_value + reg_Y_value;
    register_file[reg_X_num] = sum & 0xFF;
    register_file[0xF] = (sum & 0xFF00) ? 1 : 0;
}

void Chip8_Device::subtract_register_Y_from_register_X()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    uint8_t reg_X_value = get_register_X_value();
    uint8_t reg_Y_value = get_register_Y_value();
    register_file[reg_X_num] = reg_X_value - reg_Y_value;     // order of operations ambiguous if reg_1_num == 0xf?
    register_file[0xF] = (reg_X_value > reg_Y_value) ? 1 : 0; // c++ "underflow" may not match chip8's intended behavior
}

void Chip8_Device::subtract_register_X_from_register_Y()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    uint8_t reg_X_value = get_register_X_value();
    uint8_t reg_Y_value = get_register_Y_value();
    register_file[reg_X_num] = reg_Y_value - reg_X_value;     // order of operations ambiguous if reg_1_num == 0xf?
    register_file[0xF] = (reg_Y_value > reg_X_value) ? 1 : 0; // c++ "underflow" may not match chip8's intended behavior
}

void Chip8_Device::right_shift_register()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    uint16_t reg_to_shift = get_register_X_value();
    register_file[0xF] = reg_to_shift & 0x01;
    register_file[reg_X_num] = reg_to_shift >> 1;
}

void Chip8_Device::left_shift_register()
{
    uint8_t reg_X_num = current_instruction.get_register_X_number();
    uint16_t reg_to_shift = get_register_X_value();
    register_file[0xF] = (reg_to_shift & 0x80) ? 1 : 0;
    register_file[reg_X_num] = reg_to_shift << 1;
}

void Chip8_Device::set_index_register_to_immediate_value()
{
    index_register = current_instruction.get_12_bit_immediate();
}

void Chip8_Device::set_PC_to_immediate_value_plus_register_value()
{
    PC = current_instruction.get_12_bit_immediate() + register_file[0x0];
}

void Chip8_Device::set_register_to_random_number_ANDed_with_immediate_value()
{
    uint8_t register_num = current_instruction.get_register_X_number();
    uint8_t immediate_value = current_instruction.get_8_bit_immediate();
    register_file[register_num] = rand() & immediate_value;
}

void Chip8_Device::draw_sprite_at_index_address_to_coordinates_in_registers()
{
    chip8_sprite sprite = get_sprite_from_memory();
    display_coordinates coordinates = get_sprite_coordinates_from_registers();

    display.draw_sprite(sprite, coordinates);
    display.update_display_window();

    set_flag_indicating_if_any_pixels_turned_off();
}

chip8_sprite Chip8_Device::get_sprite_from_memory()
{
    return {
        .horizontal_lines = &memory[index_register],
        .height = current_instruction.get_4_bit_immediate()};
}

display_coordinates Chip8_Device::get_sprite_coordinates_from_registers()
{
    return {
        .x = get_register_X_value(),
        .y = get_register_Y_value()};
}

void Chip8_Device::set_flag_indicating_if_any_pixels_turned_off()
{
    if (display.at_least_one_pixel_turned_off_when_last_sprite_was_drawn)
    {
        register_file[0xf] = 1;
    }
    else
    {
        register_file[0xf] = 0;
    }
}

void Chip8_Device::skip_instruction_if_key_pressed()
{
    skip_instruction_if(key_specified_in_register_is_pressed());
}
void Chip8_Device::skip_instruction_if_key_not_pressed()
{
    skip_instruction_if(!key_specified_in_register_is_pressed());
}

bool Chip8_Device::key_specified_in_register_is_pressed()
{
    uint8_t register_value = get_register_X_value();
    Chip8_Keypad::Key key_in_register = (Chip8_Keypad::Key)register_value;
    return keypad.key_is_pressed(key_in_register);
}

void Chip8_Device::set_register_to_delay_timer_value()
{
    uint8_t register_num = current_instruction.get_register_X_number();
    register_file[register_num] = delay_timer;
}

void Chip8_Device::set_delay_timer_to_register_value()
{
    delay_timer = get_register_X_value();
}

void Chip8_Device::set_sound_timer_to_register_value()
{
    sound_timer = get_register_X_value();
}

void Chip8_Device::add_register_value_to_index_register()
{
    index_register += get_register_X_value();
}

void Chip8_Device::wait_for_keypress()
{
    if (keypad.any_key_is_pressed())
    {
        set_register_to_pressed_key();
    }
    else
    {
        decrement_PC();
    }
}

void Chip8_Device::set_register_to_pressed_key()
{
    Chip8_Keypad::Key pressed_key = keypad.get_pressed_key();
    uint8_t key_number = static_cast<uint8_t>(pressed_key);
    uint8_t register_number = current_instruction.get_register_X_number();
    register_file[register_number] = key_number;
}

void Chip8_Device::set_index_register_to_address_of_font_specified_in_register()
{
    uint8_t font_character = get_register_X_value() & 0xF;
    uint16_t font_character_address = get_font_memory_address(font_character);
    index_register = font_character_address;
}

uint16_t Chip8_Device::get_font_memory_address(uint8_t font_character)
{
    return font_character * FONT_SPRITE_HEIGHT;
}

void Chip8_Device::store_register_value_digits_in_memory()
{
    uint8_t register_value = get_register_X_value();

    uint8_t hundreds_digit = register_value / 100;
    uint8_t tens_digit = (register_value / 10) % 10;
    uint8_t ones_digit = register_value % 10;

    // printf("%d %d %d %d", register_value, hundreds_digit, tens_digit, ones_digit);

    memory[index_register] = hundreds_digit;
    memory[index_register + 1] = tens_digit;
    memory[index_register + 2] = ones_digit;
}

void Chip8_Device::store_register_values_in_memory()
{
    uint8_t last_register_number_to_store = current_instruction.get_register_X_number();
    for (auto i = 0; i <= last_register_number_to_store; i++)
    {
        memory[index_register + i] = register_file[i];
    }
}

void Chip8_Device::load_memory_values_into_registers()
{
    uint8_t last_register_number_to_store = current_instruction.get_register_X_number();
    for (auto i = 0; i <= last_register_number_to_store; i++)
    {
        register_file[i] = memory[index_register + i];
    }
}