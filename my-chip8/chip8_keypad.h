#pragma once

#include <map>
#include <SDL.h>

class Chip8_Keypad
{
public:
    enum class Key
    {
        K_0,
        K_1,
        K_2,
        K_3,
        K_4,
        K_5,
        K_6,
        K_7,
        K_8,
        K_9,
        K_A,
        K_B,
        K_C,
        K_D,
        K_E,
        K_F,
    };

    Chip8_Keypad();
    bool key_is_pressed(Key key);
    bool any_key_is_pressed();
    Key get_pressed_key();
    void update_key_statuses();
    void print_key_statuses();

private:
    static const int NUM_KEYS_ON_KEYPAD = 16;

    const Key possible_key_values[NUM_KEYS_ON_KEYPAD] = {
        Key::K_0,
        Key::K_1,
        Key::K_2,
        Key::K_3,
        Key::K_4,
        Key::K_5,
        Key::K_6,
        Key::K_7,
        Key::K_8,
        Key::K_9,
        Key::K_A,
        Key::K_B,
        Key::K_C,
        Key::K_D,
        Key::K_E,
        Key::K_F,
    };

    bool key_status[NUM_KEYS_ON_KEYPAD];
    std::map<Key, SDL_Scancode> keypad_mapping;
};