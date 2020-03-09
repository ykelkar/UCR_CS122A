/*
 * GccApplication1.c
 *
 * Created: 10/5/2017 7:54:14 PM
 *  Author: student
 */ 


#include <avr/io.h>
#include "usart_ATmega1284.h"
//#include <interrupt.h>

//unsigned char temp = 0x00;
int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	initUSART(0);
	USART_Flush(0);
    while(1)
    {
		
		if (USART_IsSendReady(0))
		{
			USART_Send(0xFF, 0);
		}
		
		PORTA = 0xFF;
		
// 		if (USART_HasReceived(0)) {
// 			//...do something...
// 			temp = USART_Receive(0);
// 		}
		
		//PORTA = temp;
        //TODO:: Please write your application code 
    }
}