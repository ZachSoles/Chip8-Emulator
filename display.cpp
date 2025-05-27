#include "display.h"

Display::Display() {
    clear_display();
}

const bool (&Display::get_display() const)[DISPLAY_HEIGHT][DISPLAY_WIDTH] { return display; }

bool Display::need_to_redraw() const { return redraw; }

void Display::reset_redraw_flag() { redraw = false; };

void Display::set_redraw_flag() { redraw = true; }

void Display::clear_display() {
    // Set all booleans within display to False
    for (int i = 0; i < DISPLAY_HEIGHT; i++) {
        for (int j = 0; j < DISPLAY_WIDTH; j++) {
            display[i][j] = false;
        }
    }
    redraw = true;
}

bool Display::draw_sprite(uint8_t x, uint8_t y, uint8_t* sprite_data, uint8_t num_bytes) {
    bool flipped_pixel_off = false;
    // Ensure initial coordinates wrap around the screen
    x = x & (DISPLAY_WIDTH - 1);
    y = y & (DISPLAY_HEIGHT - 1);

    // Iterate through all rows for sprite
    for (int row_idx = 0; row_idx < num_bytes; row_idx++) {
        uint8_t current_y = y + row_idx;
        // Get sprite byte from memory
        uint8_t sprite_byte = sprite_data[row_idx];

        // Break out of loop if hit edge of screen
        if (current_y >= DISPLAY_HEIGHT) {
            break;
        }

        // Iterate through all columns within the current row
        uint8_t current_x = x;
        for (int j = 0; j < 8; j++) {
            // Break out of loop if hit edge of screen
            if (current_x >= DISPLAY_WIDTH) {
                break;
            }

            // Get if sprite pixel should be on or off
            bool sprite_bit = ((sprite_byte >> (7 - j)) & 1) == 1;

            if (sprite_bit) {
                // If the sprite pixel is on, and the display is currently on. Turn off pixel and set flag register
                if (display[current_y][current_x]) {
                    display[current_y][current_x] = false;
                    flipped_pixel_off = true;
                } else {
                    // Turn on display pixel
                    display[current_y][current_x] = true;
                }
            }
            current_x += 1;
        }
    }

    redraw = true;
    return flipped_pixel_off;
}
