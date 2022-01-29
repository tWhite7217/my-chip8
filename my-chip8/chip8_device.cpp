#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <chrono>

#include "chip8_device.h"

using namespace std::chrono_literals;

const auto ONE_SECOND_IN_NANOSECONDS = 1000000000ns;
const auto CLOCK_CYCLE_PERIOD = ONE_SECOND_IN_NANOSECONDS / 700;
const auto TIMER_PERIOD = ONE_SECOND_IN_NANOSECONDS / 60;
const auto DISPLAY_PERIOD = ONE_SECOND_IN_NANOSECONDS / 60;
const auto CLOCK_CYCLES_PER_TIMER_PERIOD = TIMER_PERIOD / CLOCK_CYCLE_PERIOD;
const auto CLOCK_CYCLES_PER_DISPLAY_PERIOD = DISPLAY_PERIOD / CLOCK_CYCLE_PERIOD;

namespace fs = std::filesystem;

Chip8_Device::Chip8_Device(Chip8_Display display) : PC{512},
                                                    index_register{0},
                                                    delay_timer{0},
                                                    sound_timer{0},
                                                    register_file{0},
                                                    display{display},
                                                    keypad{Chip8_Keypad()},
                                                    clock_cycles_since_delay_timer_decremented{0},
                                                    clock_cycles_since_sound_timer_decremented{0},
                                                    clock_cycles_since_display_updated{0}
{
    srand(time(NULL));
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
}

void Chip8_Device::cpu_tick()
{
    auto clock_before_instruction = std::chrono::system_clock::now();

    update_keypad_state();
    read_instruction_at_PC();
    increment_PC();
    perform_instruction();
    handle_display();
    handle_timers();

    auto clock_after_instruction = std::chrono::system_clock::now();
    auto instruction_run_time = clock_after_instruction - clock_before_instruction;

    auto time_to_delay = CLOCK_CYCLE_PERIOD - instruction_run_time;

    if (time_to_delay > 0ns)
    {
        std::this_thread::sleep_for(time_to_delay);
    }
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

void Chip8_Device::decrement_PC()
{
    PC -= MEMORY_ADDRESSES_PER_INSTRUCTION;
}

void Chip8_Device::update_keypad_state()
{
    keypad.update_key_statuses();
}

void Chip8_Device::print_device_state_info()
{
    printf("PC: %d\n", PC);
    printf("instruction: %04x\n", current_instruction.bits);
    printf("index register: %4d\n", index_register);
    printf("delay timer: %3d\n", delay_timer);
    printf("sound timer: %3d\n", sound_timer);
    printf("clock_cycles_since_delay_timer_decremented: %3d\n", clock_cycles_since_delay_timer_decremented);
    printf("clock_cycles_since_sound_timer_decremented: %3d\n", clock_cycles_since_sound_timer_decremented);
    for (int i = 0; i < NUMBER_OF_REGISTERS; i++)
    {
        printf("V%d: 0x%02x\n", i, register_file[i]);
    }
    // int memory_step = 4;
    // for (int i = 512; i < 700; i += memory_step)
    // {
    //     printf("%d-%d: %02x %02x %02x %02x\n", i, i + memory_step, memory[i], memory[i + 1], memory[i + 2], memory[i + 3]);
    // }
    // display.print_display_to_console();
    keypad.print_key_statuses();
}

void Chip8_Device::quit()
{
    display.close();
}

uint8_t Chip8_Device::get_register_X_value()
{
    return register_file[current_instruction.get_register_X_number()];
}

uint8_t Chip8_Device::get_register_Y_value()
{
    return register_file[current_instruction.get_register_Y_number()];
}

void Chip8_Device::handle_display()
{
    if (clock_cycles_since_display_updated >= CLOCK_CYCLES_PER_DISPLAY_PERIOD)
    {

        display.update_display_window();
        clock_cycles_since_display_updated = 0;
    }
    else
    {
        clock_cycles_since_display_updated++;
    }
}

void Chip8_Device::handle_timers()
{
    handle_timer(&delay_timer, &clock_cycles_since_delay_timer_decremented);
    handle_timer(&sound_timer, &clock_cycles_since_sound_timer_decremented);
}

void Chip8_Device::handle_timer(uint8_t *timer, int *clock_cycles_since_timer_decremented)
{
    if (*timer > 0)
    {
        if (*clock_cycles_since_timer_decremented >= CLOCK_CYCLES_PER_TIMER_PERIOD)
        {
            *timer -= 1;
            *clock_cycles_since_timer_decremented = 0;
        }
        else
        {
            *clock_cycles_since_timer_decremented += 1;
        }
    }
}

void Chip8_Device::perform_instruction()
{
    using T = Instruction::Type;
    switch (current_instruction.get_instruction_type())
    {
    case T::CLEAR_DISPLAY:
        clear_display();
        break;

    case T::RETURN_FROM_SUBROUTINE:
        return_from_subroutine();
        break;

    case T::SET_PC:
        set_PC_to_immediate_value();
        break;

    case T::ENTER_SUBROUTINE:
        enter_subroutine_at_immediate_value();
        break;

    case T::SKIP_IF_REGISTER_EQUALS_IMMEDIATE:
        skip_instruction_if_register_equals_immediate();
        break;

    case T::SKIP_IF_REGISTER_NOT_EQUAL_TO_IMMEDIATE:
        skip_instruction_if_register_not_equal_to_immediate();
        break;

    case T::SKIP_IF_REGISTERS_EQUAL:
        skip_instruction_if_registers_equal();
        break;

    case T::SKIP_IF_REGISTERS_NOT_EQUAL:
        skip_instruction_if_registers_not_equal();
        break;

    case T::SET_REGISTER_TO_IMMEDIATE:
        set_register_to_immediate_value();
        break;

    case T::ADD_IMMEDIATE_TO_REGISTER:
        add_immediate_value_to_register();
        break;

    case T::SET_REGISTER_X_TO_REGISTER_Y:
        set_register_X_to_register_Y_value();
        break;

    case T::OR_REGISTERS:
        OR_registers();
        break;

    case T::AND_REGISTERS:
        AND_registers();
        break;

    case T::XOR_REGISTERS:
        XOR_registers();
        break;

    case T::ADD_REGISTERS:
        add_registers();
        break;

    case T::SUBTRACT_REGISTER_Y_FROM_REGISTER_X:
        subtract_register_Y_from_register_X();
        break;

    case T::RIGHT_SHIFT_REGISTER:
        right_shift_register();
        break;

    case T::SUBTRACT_REGISTER_X_FROM_REGISTER_Y:
        subtract_register_X_from_register_Y();
        break;

    case T::LEFT_SHIFT_REGISTER:
        left_shift_register();
        break;

    case T::SET_INDEX_TO_IMMEDIATE_VALUE:
        set_index_register_to_immediate_value();
        break;

    case T::SET_PC_TO_IMMEDIATE_PLUS_REGISTER:
        set_PC_to_immediate_value_plus_register_value();
        break;

    case T::SET_REGISTER_TO_RANDOM:
        set_register_to_random_number_ANDed_with_immediate_value();
        break;

    case T::DRAW_SPRITE:
        draw_sprite_at_index_address_to_coordinates_in_registers();
        break;

    case T::SKIP_IF_KEY_PRESSED:
        skip_instruction_if_key_pressed();
        break;

    case T::SKIP_IF_KEY_NOT_PRESSED:
        skip_instruction_if_key_not_pressed();
        break;

    case T::SET_REGISTER_TO_DELAY_TIMER:
        set_register_to_delay_timer_value();
        break;

    case T::SET_DELAY_TIMER:
        set_delay_timer_to_register_value();
        break;

    case T::SET_SOUND_TIMER:
        set_sound_timer_to_register_value();
        break;

    case T::ADD_REGISTER_TO_INDEX:
        add_register_value_to_index_register();
        break;

    case T::WAIT_FOR_KEY_PRESS:
        wait_for_keypress();
        break;

    case T::SET_INDEX_TO_FONT_ADDRESS:
        set_index_register_to_address_of_font_specified_in_register();
        break;

    case T::STORE_DIGITS:
        store_register_value_digits_in_memory();
        break;

    case T::STORE:
        store_register_values_in_memory();
        break;

    case T::LOAD:
        load_memory_values_into_registers();
        break;

    default:
        break;
    }
}