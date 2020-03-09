/*
 * GccApplication8.c
 *
 * Created: 10/29/2017 3:40:40 PM
 * Author : Avanti
 */ 

#include <avr/io.h>

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


void InitADC(void)
{
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); // ENABLE ADC, PRESCALER
}

uint16_t readadc(uint8_t ch)
{
	ch &= 0b00000111;				//ADNING tp limit input to 7
	ADMUX = (ADMUX & 0xf8) | ch;	//Clear last 3 bits of ADMUX, OR with ch
	ADCSRA |= (1 << ADSC);			//START CONVERSION
	while ((ADCSRA) & (1 << ADSC));	//WAIT UNTIL CONVERSION IS COMPLETE
	return (ADC);					//RETURN ADC VALUE
}

unsigned char col = 0x80;
unsigned char row = 0xFE;

enum States {Init, Display} state;
void Tick_Shift()
{
	uint16_t x,y;
	switch (state)
	{
		case Init: 
			state = Display;
			break;
			
		case Display:
			state = Display;
			break;
	}	
	switch (state)
	{
		case Init:
			PORTD = col;
			PORTB = row;
			break;
			
		case Display:
			y = readadc(1);
			x = readadc(0);
			if (x > 575)
			{
				row = (row >> 1);
			}
			else if (x < 200)
			{
				row = (row << 2);
			}
			PORTD = col;
			PORTB = row;
			break;
	}
}
int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
    /* Replace with your application code */
	InitADC();
	
	state = Init;
	
	TimerSet(1000);
	TimerOn();
	
    while (1) 
    {
		Tick_Shift();
		while(!TimerFlag);
		TimerFlag = 0;
    }
	
	return 0;
}

