#include <avr/io.h>
#include <avr/interrupt.h>
//#include "timer.h"

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

// ====================
// SM1: DEMO LED matrix
// ====================
enum SM1_States {init, sm1_display} state;
	
unsigned char count = 0x00;

int SM1_Tick(int state) {
	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern

	// === Transitions ===
	switch (state) {
		case init:
			state = sm1_display;
		case sm1_display: 
			state = sm1_display;
			break;
		default: 
			state = sm1_display;
			break;
	}

	// === Actions ===
	switch (state) {
		
		case init:
			PORTA = 0x01;
			PORTB = 0x7F;
			break;
			
		case sm1_display: // If illuminated LED in bottom right corner
			if (count == 0x00)
			{
				column_sel = 0x7F;
				column_val = 0x01;
			}
			else if (column_sel == 0xFE && column_val == 0x80) 
			{
				column_sel = 0x7F; // display far left column
				column_val = 0x01; // pattern illuminates top row
			}
			// else if far right column was last to display (grounded)
			else if (column_sel == 0xFE) 
			{
				column_sel = 0x7F; // resets display column to far left column
				column_val = column_val << 1; // shift down illuminated LED one row
			}
			// else Shift displayed column one to the right
			else 
			{
				column_sel = (column_sel >> 1) | 0x80;
			}
			count++;
			break;
		default: 
			break;
	}

	PORTD = column_val; // PORTA displays column pattern
	PORTB = column_sel; // PORTB selects column to display pattern
	return state;
}

typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

int main()
{
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	
	state = init;
	
	unsigned long int SM1Tick_period = 50;
	unsigned long int SM1Tick_tick = 5;
	
	static task task1;
	
	task *tasks[] = {&task1};
		
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;//Task initial state.
	task1.period = SM1Tick_period;//Task Period.
	task1.elapsedTime = SM1Tick_period;//Task current elapsed time.
	task1.TickFct = &SM1_Tick;//Function pointer for the tick.
	
	TimerSet(SM1Tick_tick);
	TimerOn();
	
	unsigned short i;
	
	while(1)
	{
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;	
}