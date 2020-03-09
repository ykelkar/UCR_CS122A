#include "USART_Bluetooth_H_file.h"						/* Include USART header file */

void initUSART()				/* USART initialize function */
{ 
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);				/* Enable USART transmitter and receiver */
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);	/* Write USCR0C for 8 bit data and 1 stop bit */ 
	UBRR0L = BAUD_PRESCALE;							/* Load UBRR0L with lower 8 bit of prescale value */
	UBRR0H = (BAUD_PRESCALE >> 8);					/* Load UBRR0H with upper 8 bit of prescale value */
}

unsigned char USART_Receive()									/* Data receiving function */
{
	while (!(UCSR0A & (1 << RXC0)));					/* Wait until new data receive */
	return(UDR0);									/* Get and return received data */ 
}

void USART_Send(char data)						/* Data transmitting function */
{
	UDR0 = data;										/* Write data to be transmitting in UDR0 */
	while (!(UCSR0A & (1<<UDRE0)));					/* Wait until data transmit and buffer get empty */
}

unsigned char USART_HasReceived()
{
	return (UCSR0A & (1 << RXC0));
}

unsigned char USART_HasTransmitted()
{
	return (UCSR0A & (1 << TXC0));
}

unsigned char USART_IsSendReady()
{
	return (UCSR0A & (1 << UDRE0));
}

void USART_SendString(char *str)					/* Send string of USART data function */ 
{
	int i=0;																	
	while (str[i]!=0)
	{
		USART_Send(str[i]);						/* Send each char of string till the NULL */
		i++;
	}
}