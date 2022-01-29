#pragma once

#include <stdint.h>

class Instruction
{
public:
    enum class Type
    {
        CLEAR_DISPLAY,
        RETURN_FROM_SUBROUTINE,
        SET_PC,
        ENTER_SUBROUTINE,
        SKIP_IF_REGISTER_EQUALS_IMMEDIATE,
        SKIP_IF_REGISTER_NOT_EQUAL_TO_IMMEDIATE,
        SKIP_IF_REGISTERS_EQUAL,
        SKIP_IF_REGISTERS_NOT_EQUAL,
        SET_REGISTER_TO_IMMEDIATE,
        ADD_IMMEDIATE_TO_REGISTER,
        SET_REGISTER_X_TO_REGISTER_Y,
        OR_REGISTERS,
        AND_REGISTERS,
        XOR_REGISTERS,
        ADD_REGISTERS,
        SUBTRACT_REGISTER_Y_FROM_REGISTER_X,
        RIGHT_SHIFT_REGISTER,
        SUBTRACT_REGISTER_X_FROM_REGISTER_Y,
        LEFT_SHIFT_REGISTER,
        SET_INDEX_TO_IMMEDIATE_VALUE,
        SET_PC_TO_IMMEDIATE_PLUS_REGISTER,
        SET_REGISTER_TO_RANDOM,
        DRAW_SPRITE,
        SKIP_IF_KEY_PRESSED,
        SKIP_IF_KEY_NOT_PRESSED,
        SET_REGISTER_TO_DELAY_TIMER,
        SET_DELAY_TIMER,
        SET_SOUND_TIMER,
        ADD_REGISTER_TO_INDEX,
        WAIT_FOR_KEY_PRESS,
        SET_INDEX_TO_FONT_ADDRESS,
        STORE_DIGITS,
        STORE,
        LOAD,
    };

    uint16_t bits;

    void set(uint16_t);
    Type get_instruction_type();
    uint8_t get_register_X_number();
    uint8_t get_register_Y_number();
    uint8_t get_4_bit_immediate();
    uint8_t get_8_bit_immediate();
    uint16_t get_12_bit_immediate();

private:
    Type get_instruction_type_of_0x0_opcode();
    Type get_instruction_type_of_0x8_opcode();
    Type get_instruction_type_of_0xE_opcode();
    Type get_instruction_type_of_0xF_opcode();
    uint8_t first_nibble;
    uint8_t second_nibble;
    uint8_t third_nibble;
    uint8_t fourth_nibble;
    uint8_t second_byte;
    uint16_t last_12_bits;
};