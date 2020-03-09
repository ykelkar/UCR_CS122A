/*
 * Lab2_Part1.c
 *
 * Created: 10/5/2017 6:02:30 PM
 *  Author: student
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
//#include "usart_ATmega1284.h"

// USART Setup Values
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#define BAUD_RATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)

////////////////////////////////////////////////////////////////////////////////
//Functionality - Initializes TX and RX on PORT D
//Parameter: usartNum specifies which USART is being initialized
//			 If usartNum != 1, default to USART0
//Returns: None
void initUSART(unsigned char usartNum)
{
	if (usartNum != 1) {
		// Turn on the reception circuitry of USART0
		// Turn on receiver and transmitter
		// Use 8-bit character sizes
		UCSR0B |= (1 << RXEN0)  | (1 << TXEN0);
		UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
		// Load lower 8-bits of the baud rate value into the low byte of the UBRR0 register
		UBRR0L = BAUD_PRESCALE;
		// Load upper 8-bits of the baud rate value into the high byte of the UBRR0 register
		UBRR0H = (BAUD_PRESCALE >> 8);
	}
	else {
		// Turn on the reception circuitry for USART1
		// Turn on receiver and transmitter
		// Use 8-bit character sizes
		UCSR1B |= (1 << RXEN1)  | (1 << TXEN1);
		UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);
		// Load lower 8-bits of the baud rate value into the low byte of the UBRR1 register
		UBRR1L = BAUD_PRESCALE;
		// Load upper 8-bits of the baud rate value into the high byte of the UBRR1 register
		UBRR1H = (BAUD_PRESCALE >> 8);
	}
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART is ready to send
//Parameter: usartNum specifies which USART is checked
//Returns: 1 if true else 0
unsigned char USART_IsSendReady(unsigned char usartNum)
{
	return (usartNum != 1) ? (UCSR0A & (1 << UDRE0)) : (UCSR1A & (1 << UDRE1));
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART has successfully transmitted data
//Parameter: usartNum specifies which USART is being checked
//Returns: 1 if true else 0
unsigned char USART_HasTransmitted(unsigned char usartNum)
{
	return (usartNum != 1) ? (UCSR0A & (1 << TXC0)) : (UCSR1A & (1 << TXC1));
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - checks if USART has recieved data
//Parameter: usartNum specifies which USART is checked
//Returns: 1 if true else 0
unsigned char USART_HasReceived(unsigned char usartNum)
{
	return (usartNum != 1) ? (UCSR0A & (1 << RXC0)) : (UCSR1A & (1 << RXC1));
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Flushes the data register
//Parameter: usartNum specifies which USART is flushed
//Returns: None
void USART_Flush(unsigned char usartNum)
{
	static unsigned char dummy;
	if (usartNum != 1) {
		while ( UCSR0A & (1 << RXC0) ) { dummy = UDR0; }
	}
	else {
		while ( UCSR1A & (1 << RXC1) ) { dummy = UDR1; }
	}
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - Sends an 8-bit char value
//Parameter: Takes a single unsigned char value
//			 usartNum specifies which USART will send the char
//Returns: None
void USART_Send(unsigned char sendMe, unsigned char usartNum)
{
	if (usartNum != 1) {
		while( !(UCSR0A & (1 << UDRE0)) );
		UDR0 = sendMe;
	}
	else {
		while( !(UCSR1A & (1 << UDRE1)) );
		UDR1 = sendMe;
	}
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - receives an 8-bit char value
//Parameter: usartNum specifies which USART is waiting to receive data
//Returns: Unsigned char data from the receive buffer
unsigned char USART_Receive(unsigned char usartNum)
{
	if (usartNum != 1) {
		while ( !(UCSR0A & (1 << RXC0)) ); // Wait for data to be received
		return UDR0; // Get and return received data from buffer
	}
	else {
		while ( !(UCSR1A & (1 << RXC1)) );
		return UDR1;
	}
}

enum States {Wait, Push, Release, Light_ON, Light_OFF, Release_2} state;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned char tmpA = 0x00;
unsigned char flag = 0;
unsigned char cnt = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char cnt2 = 0;

void Tick() {
	unsigned char button = ~PINB & 0x01;
	unsigned char pa = ~(PINB);
	
	switch(state)
	{
		case Wait:
			state = Push;
			break;
		
		case Push:
			if(button) state = Release;
			break;
		
		case Release:
			if(!button) state = Light_ON;
			break;
			/*
		case Stall:
			if (cnt == 0)
			{
				cnt++;
				state = Light_ON;
			}
			else if (cnt == 1)
			{
				cnt--;
				state = Light_OFF;
			}
			break;
		*/
		case Light_ON:
			if(!button && cnt == 9)
			{
				cnt = 0;
				state = Light_OFF;
			}
			else if (!button && cnt < 9)
			{
				cnt++;
			}
			else if (button) {
				state = Release_2;
			}
			break;
		
		case Light_OFF:
			if(!button && cnt == 9)
			{
				cnt = 0;
				 state = Light_ON;
			}
			else if (!button && cnt < 9)
			{
				cnt++;
			}
			else if (button) {
				state = Release_2;
			}
			break;
		
		case Release_2:
			if(!button) state = Wait;
			break;
		
		default:
			break;
	}
	switch(state)
	{
		case Wait:
			break;
		
		case Push:
			if (USART_HasReceived(0))
			{
				tmpA = USART_Receive(0);
				PORTA = tmpA;
				USART_Flush(0);
			}
			break;
		
		case Release:
			break;
			
		
		case Light_ON:
			PORTC = 0x01;
			tmpA = 0x01;
			if(USART_IsSendReady(0))
			{
				USART_Send(tmpA, 0);
			}
			if(USART_HasTransmitted(0))
			{
				PORTA = tmpA;
			}
			else
			{
				PORTA = 0x01;
			}
			break;
		
		case Light_OFF:
			PORTC = 0x01;
			tmpA = 0x00;
			if(USART_IsSendReady(0))
			{
				USART_Send(tmpA, 0);
			}
			if(USART_HasTransmitted(0))
			{
				PORTA = tmpA;
			}
			else
			{
				PORTA = 0x00;
			}
			break;
		
		case Release_2:
			PORTA = 0x00;
			tmpA = 0x00;
			PORTC = 0x00;
			break;
		default:
			break;
	}
}


int main(void)
{
	DDRA = 0xFF; DDRC = 0xFF; DDRB = 0x00;
	PORTA = 0x00; PORTC = 0x00; PORTB = 0xFF;
	TimerSet(100);
	TimerOn();
	initUSART(0);
    while(1)
    {
		Tick();
		while(!TimerFlag);
		TimerFlag = 0;	
    }
}