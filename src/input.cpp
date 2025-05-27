#include "input.h"
#include <iostream> // For error/debug output

Input::Input() {
    // Initialize all Chip-8 key states to not pressed
    key_states.fill(false);
    last_pressed_key = -1; // No key pressed initially
    quit_requested = false;

    // Initialize the specific SDL_Scancode to Chip-8 key mapping
    initialize_key_map();
}

Input::~Input() {
    // No specific SDL cleanup needed here for input, as it's handled by SDL_Quit()
}

void Input::initialize_key_map() {
    // This mapping connects a physical keyboard key (SDL_SCANCODE)
    // to a virtual Chip-8 key (0-F).
    // You can customize this mapping as you see fit.

    // Chip-8 Key -> SDL_Scancode
    key_map[0x1] = SDL_SCANCODE_1;
    key_map[0x2] = SDL_SCANCODE_2;
    key_map[0x3] = SDL_SCANCODE_3;
    key_map[0xC] = SDL_SCANCODE_4; // Mapped to 4 for logical layout

    key_map[0x4] = SDL_SCANCODE_Q;
    key_map[0x5] = SDL_SCANCODE_W;
    key_map[0x6] = SDL_SCANCODE_E;
    key_map[0xD] = SDL_SCANCODE_R; // Mapped to R

    key_map[0x7] = SDL_SCANCODE_A;
    key_map[0x8] = SDL_SCANCODE_S;
    key_map[0x9] = SDL_SCANCODE_D;
    key_map[0xE] = SDL_SCANCODE_F; // Mapped to F

    key_map[0xA] = SDL_SCANCODE_Z;
    key_map[0x0] = SDL_SCANCODE_X; // Chip-8 0 is often mapped to X
    key_map[0xB] = SDL_SCANCODE_C;
    key_map[0xF] = SDL_SCANCODE_V; // Mapped to V
}

void Input::poll_events() {
    SDL_Event event;
    last_pressed_key = -1; // Reset last_pressed_key at the start of each cycle

    // Loop through all pending SDL events
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit_requested = true;
                break;

            case SDL_KEYDOWN:
                // Ignore key repeats for most Chip-8 games
                if (event.key.repeat == 0) {
                    for (int i = 0; i < CHIP8_KEY_COUNT; ++i) {
                        if (key_map[i] == event.key.keysym.scancode) {
                            key_states[i] = true;
                            last_pressed_key = i; // Store the key that was just pressed
                            break; // Found the mapping, no need to check further
                        }
                    }
                }
                break;

            case SDL_KEYUP:
                for (int i = 0; i < CHIP8_KEY_COUNT; ++i) {
                    if (key_map[i] == event.key.keysym.scancode) {
                        key_states[i] = false;
                        break; // Found the mapping, no need to check further
                    }
                }
                break;

            default:
                // Handle other event types if necessary (e.g., window resize, mouse)
                break;
        }
    }
}

bool Input::is_pressed(uint8_t chip8_key_code) const {
    if (chip8_key_code >= 0 && chip8_key_code < CHIP8_KEY_COUNT) {
        return key_states[chip8_key_code];
    }
    std::cerr << "Warning: Attempted to check invalid Chip-8 key code: "
              << static_cast<int>(chip8_key_code) << std::endl;
    return false; // Invalid key code
}

int Input::get_pressed_key() const {
    return last_pressed_key;
}

bool Input::should_quit() const {
    return quit_requested;
}
