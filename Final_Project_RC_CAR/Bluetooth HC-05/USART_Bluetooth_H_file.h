#ifndef USART_BLUETOOTH_H_FILE_H_				/* Define library H file if not defined */
#define USART_BLUETOOTH_H_FILE_H_

#define F_CPU 8000000UL						/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>
#define BAUD_RATE 9600				/* Include AVR std. library file */
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)	/* Define prescale value */

void initUSART();				/* USART initialize function */
unsigned char USART_Receive();						/* Data receiving function */
void USART_Send(char);					/* Data transmitting function */
unsigned char USART_HasReceived();
unsigned char USART_HasTransmitted();
unsigned char USART_IsSendReady();
void USART_SendString(char*);				/* Send string of USART data function */


#endif										/* USART_ATMEGA_H_FILE_H_ */