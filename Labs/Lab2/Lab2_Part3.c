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

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

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

unsigned long int findGCD (unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1)
	{
		c = a % b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

typedef struct _task
{
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
} task;

unsigned char tmp = 0x00;
unsigned long cnt = 0;
unsigned char flag = 0;

enum States {Master, Slave} state;
	
void Tick() {
	
	switch(state) //Transitions
	{
		case Master:
			if (!USART_IsSendReady(1) || !USART_HasReceived(0))
			{
				flag = 1;
				state = Master;
			}
			else if (USART_HasReceived(0))
			{
				flag = 0;
				state = Slave;
			}
			break;
			
		case Slave:
			cnt++;
			if (cnt >= 300)
			{
				cnt = 0;
				flag = 1;
				state = Master;
			}
			else if (cnt < 300)
			{
				if (USART_HasReceived(0))
				{
					flag = 0;
					cnt = 0;
					state = Slave;
				}
				else
				{
					flag = 0;
					state = Slave;
				}
			}
			break;
		
		default:
			break;
	}
	
	switch(state) //Actions
	{
		case Master:
			if (USART_IsSendReady(1))
			{
				USART_Send(tmp, 1);
				PORTA = tmp;
			}
			PORTC = 0x01;
			break;
		
		case Slave:
			if (USART_HasReceived(0))
			{
				PORTA = USART_Receive(0);
				USART_Flush(0);
			}
			PORTC = 0x00;
			break;
		
		default:
			break;
	}
}

enum Light_States {Light_ON, Light_OFF} light_state;

void Tick2()
{
	switch(light_state) //Trans
	{
		case Light_ON:
			if (flag == 1)
			{
				tmp = ~tmp;
				light_state = Light_ON;
			}
			else
			{
				light_state = Light_OFF;
			}
			break;
			
		case Light_OFF:
			if (flag == 0)
			{
				light_state = Light_OFF;
			}
			else
			{
				light_state = Light_ON;
			}
			break;
		
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0xFF; DDRC = 0xFF; 
	PORTA = 0x00; PORTC = 0x00; 
	
	unsigned long int Master_calc = 10;
	unsigned long int light_calc = 1000;
	unsigned char i = 0;
	unsigned long int tmpGCD = 10;
	tmpGCD = findGCD(Master_calc, light_calc);

	unsigned long int GCD = tmpGCD;
	
	static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = Slave;
	task1.period = Master_calc;
	task1.elapsedTime = Master_calc;
	task1.TickFct = &Tick;

	task2.state = Light_OFF;
	task2.period = light_calc;
	task2.elapsedTime = light_calc;
	task2.TickFct = &Tick2;
	
	TimerSet(GCD);
	TimerOn();
	
	initUSART(0);
	initUSART(1);
	
    while(1)
    {
		
		for ( i = 0; i < numTasks; i++ )
		{
			if ( tasks[i]->elapsedTime == tasks[i]->period )
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;
		}

		while(!TimerFlag);
		TimerFlag = 0;	
    }
}