// main.cpp

#include "cpu.h"
#include "input.h"
#include "display.h"

#include <SDL.h>     // Include SDL header
#include <iostream>  // For error output
#include <chrono>    // For timing
#include <vector>    // For loading ROM
#include <fstream>   // Required for file operations

// Define emulator constants (should ideally be in a common header or here)
const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;
const int PIXEL_SCALE = 10; // How much to scale each Chip-8 pixel on screen

// Function to load a Chip-8 ROM file into a vector of bytes
std::vector<uint8_t> load_rom_file(const std::string& filepath) {
    // Open the file in binary mode and at the end to get its size
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open ROM file: " << filepath << std::endl;
        return {}; // Return an empty vector to indicate failure
    }

    // Get the file size
    std::streamsize file_size = file.tellg();
    if (file_size < 0) { // Check for potential errors with tellg
        std::cerr << "Error: Could not determine file size for ROM: " << filepath << std::endl;
        return {};
    }

    // Seek back to the beginning of the file
    file.seekg(0, std::ios::beg);

    // Create a vector of uint8_t with the exact size of the file
    std::vector<uint8_t> rom_data(static_cast<size_t>(file_size));

    // Read the entire file content into the vector
    // reinterpret_cast<char*> is needed because std::ifstream::read expects a char* buffer
    if (!file.read(reinterpret_cast<char*>(rom_data.data()), file_size)) {
        std::cerr << "Error: Could not read ROM file: " << filepath << std::endl;
        return {}; // Return an empty vector on read failure
    }

    file.close(); // Close the file

    std::cout << "Successfully loaded ROM: " << filepath << " (" << file_size << " bytes)" << std::endl;
    return rom_data;
}

int main(int argc, char* argv[]) {

    // 1. Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 2. Create SDL Window and Renderer
    SDL_Window* window = SDL_CreateWindow(
        "Chip-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WIDTH * PIXEL_SCALE, CHIP8_HEIGHT * PIXEL_SCALE,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 3. Initialize Chip-8 components
    Input input;
    Display display; // Display object will manage its own pixel buffer
    CPU cpu;

    // Seed random number generator once at the start for the CPU's rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    // 4. Load ROM
    const std::string rom_filepath = "rom.rom";
    std::vector<uint8_t> rom_data = load_rom_file(rom_filepath);
    if (rom_data.empty()) {
        std::cerr << "Exiting due to ROM loading failure." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    std::cout << "Loading ROM file into memory..." << std::endl;
    cpu.load_program(rom_data.data(), rom_data.size());
    std::cout << "Done loading file into memory"  << std::endl;;

    // 5. Main Emulation Loop Setup
    bool running = true;
    const int CPU_CYCLES_PER_SECOND = 600; // Typically around 500-700 Hz for Chip-8
    const int TIMER_HZ = 60; // Timers decrement at 60Hz

    // Calculate duration for one CPU cycle
    const std::chrono::nanoseconds cycle_duration(1000000000 / CPU_CYCLES_PER_SECOND);
    auto last_cycle_time = std::chrono::high_resolution_clock::now();

    // To handle 60Hz timer updates
    auto last_timer_update_time = std::chrono::high_resolution_clock::now();
    const std::chrono::nanoseconds timer_update_duration(1000000000 / TIMER_HZ);

    // 6. Emulation Loop
    std::cout << "Starting emulation..."  << std::endl;
    while (running) {
        // Process SDL Events through the Input class
        input.poll_events();

        // Check for quit request from Input class
        if (input.should_quit()) {
            running = false;
            break;
        }

        // Handle Fx0A (LD Vx, K) opcode: CPU pauses until a key is pressed
        if (cpu.is_paused()) {
            int pressed_key = input.get_pressed_key();
            if (pressed_key != -1) {
                cpu.set_register_after_key_press(pressed_key); // Assumes this method exists in CPU
                cpu.unpause();
            }
        } else {
            // Emulate CPU cycles based on target speed
            auto current_time = std::chrono::high_resolution_clock::now();
            if (current_time - last_cycle_time >= cycle_duration) {
                cpu.emulate_cycle(display, input);
                last_cycle_time = current_time;
            }
        }

        // Update timers at 60Hz
        auto current_time_timer = std::chrono::high_resolution_clock::now();
        if (current_time_timer - last_timer_update_time >= timer_update_duration) {
            // Only decrement timers if CPU is not paused.
            // This behavior varies slightly between emulators;
            // some decrement always, others only when not paused for Fx0A.
            // Sticking to common practice where Fx0A truly "pauses" everything.
            if (!cpu.is_paused()) {
                 cpu.decrement_timers(); // You'll need to add this method to your CPU
            }
            last_timer_update_time = current_time_timer;
        }


        // Rendering
        if (display.need_to_redraw()) {
            // Clear SDL renderer to black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Get the Chip-8 display pixel data
            const auto& pixels = display.get_display(); // Get const reference to bool[][]

            // Set drawing color to white for pixels
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            // Draw each active Chip-8 pixel as a scaled SDL_Rect
            for (int y = 0; y < CHIP8_HEIGHT; ++y) {
                for (int x = 0; x < CHIP8_WIDTH; ++x) {
                    if (pixels[y][x]) {
                        SDL_Rect pixel_rect = {x * PIXEL_SCALE, y * PIXEL_SCALE, PIXEL_SCALE, PIXEL_SCALE};
                        SDL_RenderFillRect(renderer, &pixel_rect);
                    }
                }
            }

            // Present the rendered content to the window
            SDL_RenderPresent(renderer);

            // Reset the redraw flag after drawing
            display.reset_redraw_flag();
        }

        // --- Frame Rate Limiting (for overall loop if needed, but per-cycle timing is better) ---
        // This can be removed if your CPU_CYCLES_PER_SECOND logic is strict enough.
        // If the CPU is very fast, you might need a small sleep here to avoid 100% CPU usage.
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // 7. Cleanup SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
