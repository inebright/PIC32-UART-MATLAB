// Bright Ine & Kwaku Kessey-Ankomah
// 11/14/2019 - ECE 216 LAB 7
// C main file to get overdamped and underdamped transient response
#include <xc.h> // PIC32 header file
#include <sys/attribs.h> // sys header file
#include <string.h> // header file to use strlen
#include <stdio.h>  // header file to use sprintf
#include "display.h" // header file containing main functions
#include "uart interface.h" // header file uart

#pragma config DEBUG = OFF          // Background Debugger disabled
#pragma config FWDTEN = OFF         // WD timer: OFF
#pragma config WDTPS = PS4096       // WD period: 4.096 sec
#pragma config POSCMOD = HS         // Primary Oscillator Mode: High Speed crystal
#pragma config FNOSC = PRIPLL       // Oscillator Selection: Primary oscillator w/ PLL
#pragma config FPLLMUL = MUL_20     // PLL Multiplier: Multiply by 20
#pragma config FPLLIDIV = DIV_2     // PLL Input Divider:  Divide by 2
#pragma config FPLLODIV = DIV_1     // PLL Output Divider: Divide by 1
#pragma config FPBDIV = DIV_1       // Peripheral Bus Clock: Divide by 1
#pragma config UPLLEN = ON          // USB clock uses PLL
#pragma config UPLLIDIV = DIV_2     // Divide 8 MHz input by 2, mult by 12 for 48 MHz
#pragma config FUSBIDIO = ON        // USBID controlled by USB peripheral when it is on
#pragma config FVBUSONIO = ON       // VBUSON controlled by USB peripheral when it is on
#pragma config FSOSCEN = OFF        // Disable second osc to get pins back
#pragma config BWP = ON             // Boot flash write protect: ON
#pragma config ICESEL = ICS_PGx2    // ICE pins configured on PGx2
#pragma config FCANIO = OFF         // Use alternate CAN pins
#pragma config FMIIEN = OFF         // Use RMII (not MII) for ethernet
#pragma config FSRSSEL = PRIORITY_6 // Shadow Register Set for interrupt priority 6

#define REVOLUTION_COUNT 4741
#define MAX 180.00
#define MIN 0.00
volatile unsigned int oldB = 0, oldA = 0, newB = 0, newA =0;
volatile int Counter = 0, currentSamples = 0, i = 0;
int maxSamples = 100, timerStart = 0;
volatile float Kp = 0, Ki = 0, dt = 0;
volatile float reference = 180, epsilon = 20.00, degree = 0;
float integral = 0, error = 0, PIoutput = 0;
float maxPeriod = 150, minPeriod = 18;

// state machine logic
void updateCounter(unsigned int newA,unsigned int newB, unsigned int oldA, unsigned int oldB){
    switch(oldA){//old encoder A
        case 1:
            switch(oldB){//old encoder B
                case 1://11
                    if((newA == 1) && (newB == 0)){//11->10
                        //increment counter
                        Counter++;
                        return;
                    }else if((newA == 0) && (newB == 1)){//11 -> 01
                        //decrement counter
                        Counter--;
                        return;
                    }    
                    break;
                case 0://10
                    if((newA == 0) && (newB == 0)){//10->00
                        //increment counter
                        Counter++;
                        return;
                    }else if((newA == 1) && (newB == 1)){//10 -> 11
                        //decrement counter
                       Counter--;
                       return;
                    }    
                    break;
            }
            break;
        case 0:
            switch(oldB){
                case 1://01
                    if((newA == 1) && (newB == 1)){//01->11
                        //increment counter
                        Counter++;
                        return;
                    }else if((newA == 0) && (newB == 0)){//01 -> 00
                        //decrement counter
                        Counter--;
                        return;
                    }    
                    break;
                case 0://00
                    if((newA == 0) && (newB == 1)){//00->01
                        //increment counter
                        Counter++;
                        return;
                    }else if((newA == 1) && (newB == 0)){//00 -> 10
                        //decrement counter
                        Counter--;
                        return;
                    }    
                    break;
            }
            break;
    }
}

float minMax(float origMin, float origMax, float newMin, float newMax, float origValue){
    return ((((origValue - origMin)/(origMax-origMin)) * (newMax-newMin)) + newMin);
}

float PIDcontroller(float reference, float angle) {
    //Calculate P,I,D 
    float output;
    error = reference - angle;
    
    //In case of error too small then stop integration 
    if (abs(error) > epsilon){
        integral = integral + (error*dt);
    }
    output = (Kp*error)+(Ki*integral); 
    output = abs(output);
    //Saturation Filter 
    if (output >= MAX) {
        output = MAX;
    } else if (output < MIN) {
        output = MIN;
    }
    output = minMax(MIN,180,minPeriod,maxPeriod,output);//min max normalization
    
    return output;
}

void __ISR(_CHANGE_NOTICE_VECTOR, IPL6SOFT) CNISR(void){
        int i = 0;
        // read encoder values
        newB = PORTGbits.RG6; // pin 10 read encoder B and update value
        newA = PORTGbits.RG7; // pin 11 read encoder A and update value      
        updateCounter(newA, newB, oldA, oldB);
        oldB = newB; // save old encoder B value
        oldA = newA; // save old encoder A value
        //LATAINV = 0b10101010;
        IFS1bits.CNIF = 0; //clear the interrupt flag          
}

void __ISR(_TIMER_1_VECTOR, IPL5SOFT) TIMER1ISR(void){  
    if((timerStart == 1) & (currentSamples < maxSamples)){ 
        //starts transmitting degree to UART
           degree = (Counter*360.00)/REVOLUTION_COUNT; // get degree
           char test[100] = ""; // array to store degree
           int num = (int) degree; // convert degree to int
           sprintf(test, "%d\r\n", num); // convert to string
           uart_write(2 ,test); // transmit to matlab
           currentSamples += 1;  // increment
        LATAINV = 0b10101010;  // status for timer 
    }           
    IFS0bits.T1IF = 0; // clear interrupt flag
}

int main(void){
    LATA = 0x00;
    DDPCON = 0; // set DDPCON register to 0
    TRISA = 0; // set TRISA registers to 0
    TRISBbits.TRISB15 = 0; // RS TRIS register to 0
    TRISDbits.TRISD4 = 0; // ENABLE register to 0
    TRISDbits.TRISD5 = 0; // RW register to 0
    // CHANGE NOTIFS
    TRISGbits.TRISG6 = 1; // CN 8 INPUT
    TRISGbits.TRISG7 = 1; // CN 9 INPUT
    // DRIVE FORWARD/BACKWARD
    TRISDbits.TRISD10 = 0; // PIN 70
    TRISDbits.TRISD11 = 0; // PIN 71
    TRISE = 0xFF00; // set LCD bits to 0 for output
    display_driver_initialize(); // initialise display
    display_driver_clear(); // clear display    
    
    // clear notification interrupts to get encoder values
    __builtin_disable_interrupts();
    INTCONbits.MVEC = 0x1; // multi vector mode
    CNCONbits.ON = 1;
    CNENbits.CNEN8 = 1;    // CN8 as change notification pin10
    CNENbits.CNEN9 = 1;    // CN9 as change notification pin11
    IPC6bits.CNIP = 6;    // interrupt priority is 5 
    IPC6bits.CNIS = 1;    // sub-priority
    IFS1bits.CNIF = 0;    // clear interrupt flag
    IEC1bits.CNIE = 1;    // enable CN interrupt  

    PR1 = 31249; // 10khz
    TMR1 = 0; //
    //add timer 3 interrupt here
    T1CONbits.TCKPS = 3; // set prescalr to 256
    T1CONbits.TGATE = 0; // not gated
    T1CONbits.TCS = 0; // pbclk input
    T1CONbits.TSYNC = 0; // asychronous clock
    T1CONbits.ON = 1; // turn on T2CON bits
    IPC1bits.T1IP = 5; // priority
    IPC1bits.T1IS = 0; // sub-priority
    IFS0bits.T1IF = 0; // clear flag
    IEC0bits.T1IE = 1; // enable
    __builtin_enable_interrupts(); // enable interrupts
    
    // output PWM signal
    T2CONbits.TCKPS = 2;
    PR2 = 999;  // 20Khz
    TMR2 = 0;   
    OC1CONbits.OCM = 0b110;
    T2CONbits.ON = 1; // turn on T2CON bits
    OC1CONbits.ON = 1; // turn on OC1CON bits
    OC1RS = 0; // duty cycle according to value of poten/temp
    OC1R = 0;
    
    //initialize  UART 1 with a baud of 32 to send data up to sizeof(float) = 32 bits
    int baud = 230400; // match baud rate to matlab
    uart_initalize(2, baud); // initialize baud
    char test1[12] = ""; // array to store incoming uart message
    char overdamp[11] = "OVERDAMPED"; // array to compare uart message
    char underdamp[11] = "UNDERDAMPED";  // array to compare uart message
    
    uart_read(2, test1, 12);  // get incoming uart message
    int length0 = strlen(test1); // length of message
    
    // compare message to change kp and ki 
    if(strcmp(test1, overdamp) == 0){
      Kp = 0.1, Ki = 0.001, dt = 0.005;
     display_driver_write(overdamp, length0); //update display driver
    }
    else if(strcmp(test1, underdamp) == 0){
      Kp = 10, Ki = 1, dt = 0.05;
      display_driver_write(underdamp, length0); //update display driver 
    }
    
    
    while(1){
      degree = (Counter*360.00)/REVOLUTION_COUNT; // get encoder angle
      PIoutput = PIDcontroller(reference, degree); // get pi controller output
      timerStart = 1; // start timer 
        
      if (error <= 0){ //move backward
           LATDbits.LATD10 = 1;
           LATDbits.LATD11 = 0;
        }
        else{ //move forward
            LATDbits.LATD10 = 0;
            LATDbits.LATD11 = 1;
        }    
        OC1RS = PIoutput;  // output pwm    
        
        char angle[10] = "";  // string to store temp value     
        sprintf(angle,"%0.2f", degree); // store angle to string
        int length = strlen(angle); //get string length  
        //display_driver_write(angle, length); //updates display with degree value
        //display_driver_delay();
        //display_driver_clear();

    }  
    
  return 0;  
}