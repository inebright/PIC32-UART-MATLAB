// Bright Ine 
// 09/27/2019 - ECE 216 LAB 4
// C header file containing functions

#ifndef LCD_DISPLAY
#define LCD_DISPLAY


void display_driver_enable();

void display_driver_delay();

void display_driver_initialize();

void display_driver_clear();

void display_driver_write(char *data, int length);

#endif