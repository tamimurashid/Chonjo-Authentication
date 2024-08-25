#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <LiquidCrystal_I2C.h>

class LCD_Display {
public:
    // Constructor
    LCD_Display(uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows);
    
    // Initialize the display
    void begin();
    
    // Print a message on a specific row
    void printMessage(const char* message, uint8_t row);

    // Scroll a message from right to left
    void scrollMessage(const char* message, const char* message1, uint8_t row);
    // where message is title for scroll message 

    void clearMessage();

private:
   LiquidCrystal_I2C lcd;
};

#endif // LCD_DISPLAY_H
