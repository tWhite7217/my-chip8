#include "chip8_keypad.h"

Chip8_Keypad::Chip8_Keypad() : key_status{}, keypad_mapping{{
                                                 {Key::K_1, SDL_SCANCODE_1},
                                                 {Key::K_2, SDL_SCANCODE_2},
                                                 {Key::K_3, SDL_SCANCODE_3},
                                                 {Key::K_C, SDL_SCANCODE_4},
                                                 {Key::K_4, SDL_SCANCODE_Q},
                                                 {Key::K_5, SDL_SCANCODE_W},
                                                 {Key::K_6, SDL_SCANCODE_E},
                                                 {Key::K_D, SDL_SCANCODE_R},
                                                 {Key::K_7, SDL_SCANCODE_A},
                                                 {Key::K_8, SDL_SCANCODE_S},
                                                 {Key::K_9, SDL_SCANCODE_D},
                                                 {Key::K_E, SDL_SCANCODE_F},
                                                 {Key::K_A, SDL_SCANCODE_Z},
                                                 {Key::K_0, SDL_SCANCODE_X},
                                                 {Key::K_B, SDL_SCANCODE_C},
                                                 {Key::K_F, SDL_SCANCODE_V},
                                             }}
{
}

bool Chip8_Keypad::key_is_pressed(Key key)
{
    int key_number = static_cast<int>(key);
    return key_status[key_number];
}

bool Chip8_Keypad::any_key_is_pressed()
{
    bool a_key_is_pressed = false;
    for (auto &key : possible_key_values)
    {
        if (key_is_pressed(key))
        {
            a_key_is_pressed = true;
        }
    }
    return a_key_is_pressed;
}

Chip8_Keypad::Key Chip8_Keypad::get_pressed_key()
{
    for (auto &key : possible_key_values)
    {
        if (key_is_pressed(key))
        {
            return key;
        }
    }
    printf("Error: Expected that a key was pressed.");
    exit(1);
}

void Chip8_Keypad::update_key_statuses()
{
    SDL_Event e;
    while ((SDL_PollEvent(&e)) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            exit(0);
        }
    }

    const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);
    for (auto const &[key, scancode] : keypad_mapping)
    {
        int key_number = static_cast<int>(key);
        key_status[key_number] = keyboard_state[scancode] ? true : false;
    }
}

void Chip8_Keypad::print_key_statuses()
{
    for (auto &key : possible_key_values)
    {
        int key_number = static_cast<int>(key);
        const char *key_status_string = key_status[key_number] ? "true" : "false";
        printf("Key %01x pressed: %s\n", key_number, key_status_string);
    }
}