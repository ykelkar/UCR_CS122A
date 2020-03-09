/*
 * Lab6_Part1.c
 *
 * Created: 10/26/2017 7:13:52 PM
 *  Author: student
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

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

uint16_t y;

enum States {Init, Wait, Backward, Forward, Halt} state;
	
void Tick()
{

	switch(state)
	{
		case Init:
			state = Wait;
			break;
		
		case Wait:
			if (y > 600)
			{
				state = Forward;
			}
			else if (y < 300)
			{
				state = Backward;
			}
			else
			{
				state = Halt;
			}
			break;
			
		case Backward:
			state = Wait;
			break;
			
		case Forward:
			state = Wait;
			break;
			
		case Halt:
			state = Wait;
			break;
			
		default:
			break;
	}
	switch(state)
	{
		case Init:
			break;
		
		case Wait:
			//PORTC = 0b00001100;
			break;
			
		case Forward:
			// counter-clockwise rotation
			PORTC = 0b00001001;     // PC0 = Low = 0
			// PC1 = High = Vcc
			// PC2 = Low = 0
			// PC3 = High = Vcc
			break;
		
		case Backward:
			// clockwise rotation
			PORTC = 0b00000110;     // PC0 = High = Vcc
			// PC1 = Low = 0
			// PC2 = High = Vcc
			// PC3 = Low = 0
			break;
		
		case Halt:
			PORTC = 0b00000000;  
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRC = 0xFF; PORTC = 0x00;
	
	state = Init;

	InitADC();
	
	TimerSet(1);
	TimerOn();
	
	
	//shift PORTD
	
    while(1)
    {		
		y = readadc(1);
		Tick();
		while(!TimerFlag);
		TimerFlag = 0;
    }
}