#ifndef __UART_H_
#define __UART_H_

void uart_init();
void Serial_Send1(unsigned char t);
void Serial_Send2(unsigned char t);
void TX1_string(char *string);		//transmit a string by USART2 
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
int putchar(int ch);


#endif
