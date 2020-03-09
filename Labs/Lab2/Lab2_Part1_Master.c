/*
 * Lab2_Part1.c
 *
 * Created: 10/5/2017 6:02:30 PM
 *  Author: student
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart_ATmega1284.h"

enum States {Start, Init} state;

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

unsigned char tmpA = 0x00;

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

void Tick() {
	switch(state)
	{
		case Start:
			state = Init;
			break;
			
		case Init:
			if (tmpA == 0x00) tmpA = 0x01;
			else if (tmpA == 0x01) tmpA = 0x00;
			state = Init;
			break;
			
		default:
			break;
	}
	switch(state)
	{
		case Start:
			break;
			
		case Init:
			if ( USART_IsSendReady(0) ) {
				USART_Send(tmpA, 0);
			}
			
			if (USART_HasTransmitted(0)) {
				PORTA = tmpA;
			}
			break;
			
		default:
			break;
	}
}


int main(void)
{
	DDRA = 0xFF;
	PORTA = 0x00;
	TimerSet(1000);
	TimerOn();
	initUSART(0);
    while(1)
    {
		Tick();
		while(!TimerFlag);
		TimerFlag = 0;	
    }
}