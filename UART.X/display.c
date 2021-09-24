// Bright Ine 
// 09/27/2019 - ECE 216 LAB 4
// C main file containing functions

#include <xc.h>
#include "display.h"

void display_driver_enable(){
    LATDbits.LATD4 = 1; // ENABLE BIT SET TO 1
    int i = 0;
    display_driver_delay();
    LATDbits.LATD4 = 0; // ENABLE BIT SET TO 0
}

void display_driver_initialize(){
    LATDbits.LATD5 = 0; // RW set to 0
    LATBbits.LATB15 = 0; // RS set to 0
    LATE = 0b00111100;  // 2 line mode, 5x10 dots
    // wait 40 micro sec
    display_driver_enable();
    display_driver_delay();
    LATDbits.LATD5 = 0; // RW set to 0
    LATBbits.LATB15 = 0; // RS set to 0
    LATE = 0b00001100;  // DISP ON, cursor OFF, blink off
    // wait 40 micro sec  
    display_driver_delay(); 
    display_driver_enable();
}

void display_driver_clear(){
    LATDbits.LATD5 = 0; // RW set to 0
    LATBbits.LATB15 = 0; // RS set to 0
    LATE = 0b00000001;  // clr display
    // wait 1.64 milli sec
    //display_driver_delay();
    display_driver_enable();
}
void display_driver_write(char *data, int length){
    //display_driver_clear();
    LATDbits.LATD5 = 0; // RW set to 0
    LATBbits.LATB15 = 1; // RS set to 0
    int i = 0;  
    for(i=0; i<length; i++){
        char A = data[i];   // get char from string
        LATE = A;   // print char
        display_driver_delay();
        display_driver_enable();    // toggle enable bit
    }    
}

// delay function
void display_driver_delay(){
    int i = 0;
    for(i = 0; i < 1000; i++){   
    } 
}