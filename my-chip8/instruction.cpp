#include <stdlib.h>
#include <stdio.h>

#include "instruction.h"

void Instruction::set(uint16_t instruction_in_bits)
{
    bits = instruction_in_bits;
    first_nibble = instruction_in_bits >> 12;
    second_nibble = (instruction_in_bits >> 8) & 0xf;
    third_nibble = (instruction_in_bits >> 4) & 0xf;
    fourth_nibble = instruction_in_bits & 0xf;
    second_byte = instruction_in_bits & 0xff;
    last_12_bits = instruction_in_bits & 0xfff;
}

Instruction::Type Instruction::get_instruction_type()
{
    uint8_t opcode = first_nibble;
    switch (opcode)
    {
    case 0x0:
        return get_instruction_type_of_0x0_opcode();

    case 0x1:
        return Type::SET_PC;

    case 0x2:
        return Type::ENTER_SUBROUTINE;

    case 0x3:
        return Type::SKIP_IF_REGISTER_EQUALS_IMMEDIATE;

    case 0x4:
        return Type::SKIP_IF_REGISTER_NOT_EQUAL_TO_IMMEDIATE;

    case 0x5:
        return Type::SKIP_IF_REGISTERS_EQUAL;

    case 0x9:
        return Type::SKIP_IF_REGISTERS_NOT_EQUAL;

    case 0x6:
        return Type::SET_REGISTER_TO_IMMEDIATE;

    case 0x7:
        return Type::ADD_IMMEDIATE_TO_REGISTER;

    case 0x8:
        return get_instruction_type_of_0x8_opcode();

    case 0xA:
        return Type::SET_INDEX_TO_IMMEDIATE_VALUE;

    case 0xB:
        return Type::SET_PC_TO_IMMEDIATE_PLUS_REGISTER;

    case 0xC:
        return Type::SET_REGISTER_TO_RANDOM;

    case 0xD:
        return Type::DRAW_SPRITE;

    case 0xE:
        return get_instruction_type_of_0xE_opcode();

    case 0xF:
        return get_instruction_type_of_0xF_opcode();

    default:
        printf("Error: unknown opcode 0x%01x.\n", opcode);
        exit(1);
    }
}

Instruction::Type Instruction::get_instruction_type_of_0x0_opcode()
{
    switch (last_12_bits)
    {
    case 0x0E0:
        return Type::CLEAR_DISPLAY;

    case 0x0EE:
        return Type::RETURN_FROM_SUBROUTINE;

    default:
        printf("Error: unknown instruction type with opcode 0x0.\n");
        exit(1);
    }
}

Instruction::Type Instruction::get_instruction_type_of_0x8_opcode()
{
    switch (fourth_nibble)
    {
    case 0x0:
        return Type::SET_REGISTER_X_TO_REGISTER_Y;

    case 0x1:
        return Type::OR_REGISTERS;

    case 0x2:
        return Type::AND_REGISTERS;

    case 0x3:
        return Type::XOR_REGISTERS;

    case 0x4:
        return Type::ADD_REGISTERS;

    case 0x5:
        return Type::SUBTRACT_REGISTER_Y_FROM_REGISTER_X;

    case 0x6:
        return Type::RIGHT_SHIFT_REGISTER;

    case 0x7:
        return Type::SUBTRACT_REGISTER_X_FROM_REGISTER_Y;

    case 0xE:
        return Type::LEFT_SHIFT_REGISTER;

    default:
        printf("Error: unknown logical operation for ox8 opcode.\n");
        exit(1);
    }
}

Instruction::Type Instruction::get_instruction_type_of_0xE_opcode()
{
    switch (second_byte)
    {
    case 0x9E:
        return Type::SKIP_IF_KEY_PRESSED;

    case 0xA1:
        return Type::SKIP_IF_KEY_NOT_PRESSED;

    default:
        printf("Error. Unkown instruction with opcode 0xE.\n");
        exit(1);
    }
}

Instruction::Type Instruction::get_instruction_type_of_0xF_opcode()
{
    switch (second_byte)
    {
    case 0x07:
        return Type::SET_REGISTER_TO_DELAY_TIMER;

    case 0x15:
        return Type::SET_DELAY_TIMER;

    case 0x18:
        return Type::SET_SOUND_TIMER;

    case 0x1E:
        return Type::ADD_REGISTER_TO_INDEX;

    case 0x0A:
        return Type::WAIT_FOR_KEY_PRESS;

    case 0x29:
        return Type::SET_INDEX_TO_FONT_ADDRESS;

    case 0x33:
        return Type::STORE_DIGITS;

    case 0x55:
        return Type::STORE;

    case 0x65:
        return Type::LOAD;

    default:
        printf("Error. Unkown instruction with opcode 0xF.\n");
        exit(1);
    }
}

uint8_t Instruction::get_register_X_number()
{
    return second_nibble;
}

uint8_t Instruction::get_register_Y_number()
{
    return third_nibble;
}

uint8_t Instruction::get_4_bit_immediate()
{
    return fourth_nibble;
}

uint8_t Instruction::get_8_bit_immediate()
{
    return second_byte;
}

uint16_t Instruction::get_12_bit_immediate()
{
    return last_12_bits;
}