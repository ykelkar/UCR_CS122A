#include <avr/io.h>
#include "USART_Bluetooth_H_file.h"	/* include USART library */

#define LED PORTB		/* connected LED on PORT pin */

int main(void)
{
	char Data_out;
	DDRB = 0xFF; PORTD = 0x00;	/* make PORT as output port */
	initUSART();	/* initialize USART with 9600 baud rate */
	LED = 0;
	
	while(1)
	{
		Data_out = 0x01;
		USART_Send(Data_out, 0);	/* send data from Bluetooth device*/
	}
}