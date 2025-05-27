#ifndef INPUT_H
#define INPUT_H

#include <SDL.h> // Include SDL header
#include <array>   // For std::array
#include <cstdint> // For uint8_t

// Define the number of Chip-8 keys
const int CHIP8_KEY_COUNT = 16;

class Input {
public:
    Input();
    ~Input();

    // Call this at the beginning of your main loop to process SDL events
    void poll_events();

    // Check if a specific Chip-8 key is currently pressed
    bool is_pressed(uint8_t chip8_key_code) const;

    // Get the value of a key that was just pressed (for Fx0A opcode)
    // Returns -1 if no key was pressed since last call, otherwise the Chip-8 key code
    int get_pressed_key() const;

    // Check if the quit event was triggered (e.g., closing the window)
    bool should_quit() const;

private:
    std::array<bool, CHIP8_KEY_COUNT> key_states; // Array to store current state of Chip-8 keys
    int last_pressed_key; // Stores the last pressed Chip-8 key for Fx0A
    bool quit_requested;   // Flag to indicate if the user wants to quit

    // Map SDL_Scancode to Chip-8 key code
    // SDL_Scancode is preferred over SDLK_Key for layout-independent input
    std::array<SDL_Scancode, CHIP8_KEY_COUNT> key_map;

    // Initialize the key mapping
    void initialize_key_map();
};

#endif // INPUT_H
