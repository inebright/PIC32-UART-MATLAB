// Bright Ine & Kwaku Kessey-Ankomah
// 11/14/2019 - ECE 216 LAB 7
// C  file containing uart functions
#include "uart interface.h"
#include <xc.h> // PIC32 header file

int periFreq = 80000000; //peripheral frequency
int divM = 16; //default value for the high baud rate generator bit =0 and divisor = 16
void uart_initalize(int uartId, int baud){
	if((uartId > 6) || (uartId < 1)){
		//id doesnt exist
		return;
	}
	
	switch(uartId){
		case 1:
			U1MODEbits.PDSEL = 0x0; // odd parity (parity bit set to make the number of 1’s odd)
			U1MODEbits.UEN = 0; //0-only use the TX and RX, or 2 - use TX, RX, CTS, and RTS
			U1STAbits.UTXEN = 1; // enable transmit
			U1STAbits.URXEN = 1; // enable receive
			U1BRG = (periFreq/(divM*baud)) - 1; // 80 M/(16*100) - 1 = 49,999 //set up the baud = pulses
			U1MODEbits.ON = 1; // turn on the uart
			break;
		case 2:
			U2MODEbits.PDSEL = 0x0; // odd parity (parity bit set to make the number of 1’s odd)
			U2MODEbits.UEN = 0; //0-only use the TX and RX, or 2 - use TX, RX, CTS, and RTS
			U2STAbits.UTXEN = 1; // enable transmit
			U2STAbits.URXEN = 1; // enable receive
			U2BRG = (periFreq/(divM*baud)) - 1; // 80 M/(16*100) - 1 = 49,999 //set up the baud = pulses
			U2MODEbits.ON = 1; // turn on the uart
			break;
		case 3:
			U3MODEbits.PDSEL = 0x0; // odd parity (parity bit set to make the number of 1’s odd)
			U3MODEbits.UEN = 0; //0-only use the TX and RX, or 2 - use TX, RX, CTS, and RTS
			U3STAbits.UTXEN = 1; // enable transmit
			U3STAbits.URXEN = 1; // enable receive
			U3BRG = (periFreq/(divM*baud)) - 1; // 80 M/(16*100) - 1 = 49,999 //set up the baud = pulses
			U3MODEbits.ON = 1; // turn on the uart
			break;
		default:
			return;
	} 
}

int uart_read(int uartId, char * message, int maxLength){	
    char data;
    int complete, num_bytes;
	switch(uartId){
		case 1:
			data = 0;
            complete = 0, num_bytes = 0;
            // loop until you get a '\r' or '\n'
            while (!complete) {
              if (U1STAbits.URXDA) { // if data is available
                data = U1RXREG;      // read the data
                if ((data == '\n') || (data == '\r')) {
                  complete = 1;
                  LATA = 0b00001111;  // led outputs D0-D3 on
                } else {
                  message[num_bytes] = data;
                  ++num_bytes;
                  // roll over if the array is too small
                  if (num_bytes >= maxLength) {
                    num_bytes = 0;
                  }
                }
              }
            }
            // end the string
            message[num_bytes] = '\0';
			break;
		case 2:
			data = 0;
            complete = 0, num_bytes = 0;
            // loop until you get a '\r' or '\n'
            while (!complete) {
              if (U2STAbits.URXDA) { // if data is available
                data = U2RXREG;      // read the data
                if ((data == '\n') || (data == '\r')) {
                     LATA = 0b00001111;  // led outputs D0-D3 on
                  complete = 1;
                } else {
                  message[num_bytes] = data;
                  ++num_bytes;
                  // roll over if the array is too small
                  if (num_bytes >= maxLength) {
                    num_bytes = 0;
                  }
                }
              }
            }
            // end the string
            message[num_bytes] = '\0';
			break;
		case 3:
			data = 0;
            complete = 0, num_bytes = 0;
            // loop until you get a '\r' or '\n'
            while (!complete) {
              if (U3STAbits.URXDA) { // if data is available
                data = U3RXREG;      // read the data
                if ((data == '\n') || (data == '\r')) {
                  complete = 1;
                } else {
                  message[num_bytes] = data;
                  ++num_bytes;
                  // roll over if the array is too small
                  if (num_bytes >= maxLength) {
                    num_bytes = 0;
                  }
                }
              }
            }
            // end the string
            message[num_bytes] = '\0';
			break;
		default:
			return;
    }
	return data;
}

void uart_write(int uartId, char * string){
	switch(uartId){
		case 1:
			while (*string != '\0') {
              while (U1STAbits.UTXBF) {
                ; // wait until tx buffer isn't full
              }
              U1TXREG = *string;
              ++string;
            }
			break;
		case 2:
			while (*string != '\0') {
            while (U2STAbits.UTXBF) {
                ; // wait until tx buffer isn't full
              }
              U2TXREG = *string;
              ++string;
            } 
			break;
		case 3:
            while (*string != '\0') {
              while (U3STAbits.UTXBF) {
                ; // wait until tx buffer isn't full
              }
              U3TXREG = *string;
              ++string;
            }
			break;
		default:
			return;
    }
	return;
}