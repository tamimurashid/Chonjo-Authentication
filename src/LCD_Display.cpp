#include "LCD_Display.h"

// Constructor implementation
LCD_Display::LCD_Display(uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows) 
    : lcd(lcdAddr, lcdCols, lcdRows) {}

// Initialize the LCD
void LCD_Display::begin() {
    lcd.begin(16, 2);  // Pass the number of columns and rows
    lcd.backlight();
}

// Print a message on a specific row
void LCD_Display::printMessage(const char* message, uint8_t row) {
    lcd.setCursor(0, row);
    lcd.print(message);
    
}
void LCD_Display::clearMessage(){
    lcd.clear();
}

// Scroll a message from right to left
void LCD_Display::scrollMessage(const char* message, const char* message1, uint8_t row) {
    int len = strlen(message);
    for (int i = 0; i < len + 16; i++) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(message1);
        lcd.setCursor(0, 0);
        lcd.setCursor(16 - i, row); // Adjust position to scroll from right to left
        lcd.print(message);
        delay(100); // Adjust scroll speed
        lcd.clear();
    }
}
