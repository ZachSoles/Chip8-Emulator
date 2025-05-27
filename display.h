#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint> // Required for uint8_t

// Constants for display dimensions.
const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;

class Display {
private:
    // Declare the private pixel buffer.
    bool display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    // Declare the private redraw flag.
    bool redraw;

public:
    Display();

    // Getter for the display pixel data.
    // Returns a const reference to the 2D boolean array.
    const bool (&get_display() const)[DISPLAY_HEIGHT][DISPLAY_WIDTH];

    // Method to check if the display needs to be redrawn.
    bool need_to_redraw() const;

    // Method to reset the redraw flag after the display has been rendered.
    void reset_redraw_flag();

    void set_redraw_flag();

    // Clears all pixels on the display (sets them to false/off).
    void clear_display();

    // Draws a sprite onto the display buffer using XOR logic.
    // x, y: The top-left coordinates on the display to start drawing.
    // sprite_data: Pointer to the sprite bytes in memory.
    // num_bytes: The height of the sprite in bytes (each byte is 8 pixels wide).
    // Returns true if any pixel was flipped from on to off (for CHIP-8's VF register).
    bool draw_sprite(uint8_t x, uint8_t y, uint8_t* sprite_data, uint8_t num_bytes);
};

#endif // DISPLAY_H
