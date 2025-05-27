#include <cstdlib>
#include <cstdint>
#include <iostream>
#include "cpu.h"
#include "input.h"
#include "display.h"

CPU::CPU() {
    program_counter = PROGRAM_BUFFER;
    index_register = 0;
    paused = false;
    delay_timer = 0;
    sound_timer = 0;
    stack_pointer = 0;
    initialize_cpu();
}

void CPU::initialize_cpu() {
    clear_registers();
    clear_stack();
    clear_memory();
    load_font();
}

bool CPU::is_paused () const { return paused; }
void CPU::unpause() { paused = false; }

void CPU::decrement_timers() {
    // Update timers if they are above 0
    if (sound_timer > 0) { sound_timer -= 1; }
    if (delay_timer > 0) { delay_timer -= 1; }
}

void CPU::clear_memory() {
    // Iterate through all memory positions and set values to 0
    for (int i = 0; i < MEMORY_COUNT; i++) {
        memory[i] = 0;
    }
}

void CPU::clear_stack() {
    // Iterate through all stack positions and set values to 0
    for (int i = 0; i < STACK_COUNT; i++) {
        stack[i] = 0;
    }
}

void CPU::clear_registers() {
    // Iterate through all regsiters and set values to 0
    for (int i = 0; i < REGISTER_COUNT; i++) {
        registers[i] = 0;
    }
}

void CPU::push_to_stack(uint16_t value) {
    // Don't push to stack if stack is full
    if (stack_pointer >= STACK_COUNT) {
        std::cerr << "Error: Pushing to a full stack" << std::endl;
        return;
    }

    // Assign value to top value within the stack
    stack[stack_pointer] = value;
    stack_pointer += 1;
}

uint16_t CPU::pop_from_stack () {
    // Don't pop from stack if stack is empty
    if (stack_pointer == 0){
        std::cerr << "Error: Popping from an empty stack" << std::endl;
        return 0;
    }

    // Get value from top of stack
    stack_pointer -= 1;
    return stack[stack_pointer];
}

void CPU::load_font() {
    // Initalize and load font (sprites) into memory
    for (int i = 0; i < FONT_COUNT; i++) {
        memory[i] = CHIP8_FONT[i];
    }
}

void CPU::load_program(uint8_t program[], int size) {
    if (PROGRAM_BUFFER + size >= MEMORY_COUNT) {
        std::cerr << "Error: Program size too large to fit in memory\n"  << std::endl;
        exit(1);
    }

    // Load a program into the CPU's memory
    for (int i = 0; i < size; i++) {
        memory[PROGRAM_BUFFER + i] = program[i];
    }
}

uint16_t CPU::fetch_opcode() {
    if (program_counter + 1 >= MEMORY_COUNT) {
        std::cerr << "Error: Attempted to fetch instruction beyond memory bounds at 0x"
                    << std::hex << program_counter << "\n";
        exit(1); // or throw an exception, or set a fatal error flag
    }


    uint8_t instruction_1 = memory[program_counter];
    uint8_t instruction_2 = memory[program_counter + 1];

    // Increment program counter
    program_counter += 2;

    // Combine the into a single 16-bit instruction
    return (instruction_1 << 8) | instruction_2;
}

void CPU::execute_opcode(uint16_t instruction, Display& display, Input& input, bool new_functionality) {
    // Extract nibbles from the instructions
    uint16_t first = (instruction & 0xF000); // Get instruction type
    uint8_t x = (instruction & 0x0F00) >> 8;      // 2nd nibble
    uint8_t y = (instruction & 0x00F0) >> 4;      // 3rd nibble
    uint8_t n = (instruction & 0x000F);           // 4th nibble
    uint8_t nn = (instruction & 0x00FF);          // 3 + 4 nibbles
    uint16_t nnn = (instruction & 0x0FFF);        // 2 + 3 + 4 nibbles

    switch (first) {
        case 0x0000:
            switch (nnn) {
                case 0x00E0:
                    // Clear the display
                    display.clear_display();
                    break;
                case 0x00EE:
                    // Set program counter to the item on the stack
                    program_counter = pop_from_stack();
                default:
                    break;
            }
            break;
        case 0x1000:
            // Set program counter to NNN
            program_counter = nnn;
            break;
        case 0x2000:
            push_to_stack(program_counter);
            program_counter = nnn;
            break;
        case 0x3000:
            if (registers[x] == nn) {
                program_counter += 2;
            }
            break;
        case 0x4000:
            if (registers[x] != nn) {
                program_counter += 2;
            }
            break;
        case 0x5000:
            if (registers[x] == registers[y]) {
                program_counter += 2;
            }
            break;
        case 0x6000:
            // Set register x to value nn
            registers[x] = nn;
            break;
        case 0x7000:
            // Add value nn to register x
            registers[x] += nn;
            break;
        case 0x8000:
            switch (n) {
                case 0x0000:
                    // Assign value within y register to x
                    registers[x] = registers[y];
                    break;
                case 0x0002:
                    // Perfom AND operation on x and y registers
                    registers[x] = registers[x] & registers[y];
                    break;
                case 0x0003:
                    // Perform XOR operation on x and y registers
                    registers[x] = registers[x] ^ registers[y];
                    break;
                case 0x0004:
                    {
                        // Add value nn to register x
                        uint16_t sum = (uint16_t) registers[x] + (uint16_t) registers[y];
                        registers[x] = (uint8_t) sum;

                        // Set flag to 1 if added value is overflowing
                        registers[FLAG_REGISTER] = 0;
                        if (sum > 255) {
                            registers[FLAG_REGISTER] = 1;
                        }
                    }
                    break;
                case 0x0005:
                    // Set flag to 1 if substracted value is underflowing
                    registers[FLAG_REGISTER] = 0;
                    if (registers[x] > registers[y]) {
                        registers[FLAG_REGISTER] = 1;
                    }

                    registers[x] = registers[x] - registers[y];
                    break;
                case 0x0007:
                    // Set flag to 1 if substracted value is underflowing
                    registers[FLAG_REGISTER] = 0;
                    if (registers[x] > registers[y]) {
                        registers[FLAG_REGISTER] = 1;
                    }

                    registers[x] = registers[y] - registers[x];
                    break;
                case 0x0006:
                    // Support CHIP-48: set register value to y
                    if (new_functionality) {
                        registers[x] = registers[y];
                    }

                    // Toggle flag register if 1 is shifted out
                    registers[FLAG_REGISTER] = 0;
                    if ((registers[x] & 0x01) != 0) {
                        registers[FLAG_REGISTER] = 1;
                    }

                    registers[x] = registers[x] >> 1;
                    break;
                case 0x000E:
                    // Support CHIP-48: set register value to y
                    if (new_functionality) {
                        registers[x] = registers[y];
                    }
                    registers[FLAG_REGISTER] = 0;

                    if ((registers[x] & 0x80) != 0) {
                        registers[FLAG_REGISTER] = 1;
                    }
                    registers[x] = registers[x] << 1;
                    break;
            }
            break;
        case 0x9000:
            // Skip to next instruction if x and y do not equal each other
            if (registers[x] != registers[y]) {
                program_counter += 2;
            }
            break;
        case 0xA000:
            // Set index register to nnn
            index_register = nnn;
            break;
        case 0xB000:
            // Support CHIP-48: new jump that jumps to xnn + the value within register x
            if (new_functionality) {
                program_counter = nnn + registers[x];
            } else {
                // Jump to the position nnn plus the value in the first register
                program_counter = nnn + registers[0];
            }
            break;
        case 0xC000:
            // Assigns register x to random value AND nn
            registers[x] = rand() & nn;
            break;
        case 0xD000:
            registers[FLAG_REGISTER] = display.draw_sprite(registers[x], registers[y], &memory[index_register], n);
            break;
        case 0xE000:
            switch (nn) {
                case 0x009E:
                    if (input.is_pressed(registers[x])) {
                        program_counter += 2;
                    }
                    break;
                case 0x00A1:
                    if (!input.is_pressed(registers[x])) {
                        program_counter += 2;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 0xF000:
            switch (nn) {
                case 0x0007:
                    // Set register to delay timer
                    registers[x] = delay_timer;
                    break;
                case 0x0015:
                    // Set delay timer to value in register
                    delay_timer = registers[x];
                    break;
                case 0x0018:
                    // Set sound timer to value in register
                    sound_timer = registers[x];
                    break;
                case 0x001E:
                    {
                        // Add to index register
                        uint16_t sum = index_register + registers[x];
                        index_register = (uint8_t) sum;
                        registers[FLAG_REGISTER] = 0;

                        // Set flag register if overflowed
                        if (sum > 0xFFF) {
                            registers[FLAG_REGISTER] = 1;
                        }
                    }
                    break;
                case 0x000A:
                    // Pause the program and wait for key input
                    paused = true;
                    paused_register = x;
                    program_counter -= 2;
                    break;
                case 0x0029:
                    // Set the index register to the font hex that exists within the register
                    index_register = registers[x] * 5;
                    break;
                case 0x0033:
                    {
                        // Break down the value within register and set the 3 numbers into memory
                        uint8_t value = registers[x];
                        memory[index_register] = value / 100;
                        memory[index_register + 1] = (value % 100) / 10;
                        memory[index_register + 2] = value % 10;
                    }
                    break;
                case 0x0055:
                    // Set memory to values within x registers
                    for (int i = 0; i <= x; i++) {
                        memory[index_register + i] = registers[i];
                    }

                    // Increment index register if not using new functionality
                    if (!new_functionality) { index_register += x + 1; }
                    break;
                case 0x0065:
                    // Set registers to values within memory
                    for (int i = 0; i <= x; i++) {
                        registers[i] = memory[index_register + i];
                    }

                    // Increment index register if not usng new functionality
                    if (!new_functionality) { index_register += x + 1; }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void CPU::set_register_after_key_press(uint8_t key_pressed) {
    registers[paused_register] = key_pressed;
}

void CPU::emulate_cycle(Display& display, Input& input) {
    if (paused) {
        return;
    }

    // Fetch the current instruction
    uint16_t instruction = fetch_opcode();

    // Decode the instruction and execute
    execute_opcode(instruction, display, input, false);
}
