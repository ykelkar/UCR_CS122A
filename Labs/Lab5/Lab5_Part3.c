#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn()
{
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff()
{
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR()
{
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0)
	{ // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char j = 0x00;
unsigned char button = 0x00;
static unsigned char row[] = {0x3C, 0x24, 0x24, 0x3C}; // sets the pattern displayed on columns
static unsigned char col[] = {0xDF, 0xEF, 0xF7, 0xFB}; // grounds column to display pattern
enum SM1_States {Init, First_Col, Second_Col, Third_Col, Fourth_Col} state;

void SM1_Tick() {
	// === Local Variables ===
	// 	unsigned char column_val;
	// 	unsigned char column_sel;
	// === Transitions ===
	// 	while (1)
	// 	{
	// 		PORTD = row[i]; // PORTD displays column pattern
	// 		PORTB = col[i]; // PORTB selects column to display pattern
	// 		j++;
	// 		if (j == 4)
	// 		{
	// 			j = 0;
	// 		}
	// 	}
	switch (state)
	{
		case Init:
		state = First_Col;
		break;
		
		case First_Col:
		state = Second_Col;
		break;
		
		case Second_Col:
		state = Third_Col;
		break;
		
		case Third_Col:
		state = Fourth_Col;
		break;
		
		case Fourth_Col:
		state = First_Col;
		break;
		
		default:
		break;
	}
	switch (state)
	{
		case Init:
		j = 0;
		PORTD = row[j];
		PORTB = col[j];
		break;
		
		case First_Col:
		j = 0;
		PORTD = row[j];
		PORTB = col[j];
		break;
		
		case Second_Col:
		j = 1;
		PORTD = row[j];
		PORTB = col[j];
		break;
		
		case Third_Col:
		j = 2;
		PORTD = row[j];
		PORTB = col[j];
		break;
		
		case Fourth_Col:
		j = 3;
		PORTD = row[j];
		PORTB = col[j];
		break;
		
		default:
		break;
	}
	
}

int main()
{
	DDRA = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0x00; PORTC = 0xFF;
	
	TimerSet(1);
	TimerOn();
	
	while(1)
	{
		SM1_Tick();
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}