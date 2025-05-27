#ifndef CPU_H
#define CPU_H

#include <cstdint> // For uint8_t and uint16_t
#include <iostream> // For std::cout in error messages (though consider removing in final build)

// Forward declarations to avoid circular dependencies if Display/Input also include CPU.h
class Display;
class Input;

// Constants are usually defined globally or as static const members within the class
// For a header, it's common to define them here if they're used by other parts of the system.
const int MEMORY_COUNT = 4096;
const int STACK_COUNT = 16;
const int REGISTER_COUNT = 16;
const int FLAG_REGISTER = REGISTER_COUNT - 1; // VF register
const int FONT_COUNT = 80;
const int PROGRAM_BUFFER = 0x200;

// Chip-8 font sprites
const uint8_t CHIP8_FONT[] = {
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

class CPU {
private:
    uint8_t registers[REGISTER_COUNT];
    uint16_t program_counter;
    uint16_t index_register;
    bool paused;
    uint8_t paused_register; // Stores the register to load key into after pause
    uint8_t memory[MEMORY_COUNT];

    // Timers
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Stack
    uint8_t stack_pointer;
    uint16_t stack[STACK_COUNT];

    // Private helper methods (these are typically not exposed)
    void clear_memory();
    void clear_stack();
    void clear_registers();
    void push_to_stack(uint16_t value);
    uint16_t pop_from_stack();
    void load_font();
    uint16_t fetch_opcode();

public:
    // Constructor
    CPU();

    // Public methods for CPU operation
    void initialize_cpu();
    bool is_paused() const;
    void unpause();
    void load_program(uint8_t program[], int size);
    void execute_opcode(uint16_t instruction, Display& display, Input& input, bool new_functionality = false);
    void emulate_cycle(Display& display, Input& input);
    void decrement_timers();
    void set_register_after_key_press(uint8_t key_pressed);
};

#endif // CPU_H
