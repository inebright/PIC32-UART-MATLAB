// Kwaku Kessey-Ankomah 
// 11/14/2019 - ECE 216 LAB 7
// C header file containing functions

void uart_initalize(int uartId, int baud);

int uart_read(int uartId, char * message, int maxLength);

void uart_write(int uartId, char * string);
