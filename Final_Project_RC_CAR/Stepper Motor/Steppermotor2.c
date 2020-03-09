#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/portpins.h>
#include <avr/pgmspace.h>

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"
enum LEDState {A, AB, B, BC, C, CD, D, DA} state;

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

uint16_t x;

void LEDS_Init()
{
	state = A;
}

void LEDS_Tick()
{
	x = readadc(0);
	
	switch(state)
	{
		case A:
		if(x > 600)
		{
			state = AB;
		}
		else if (x < 400)
		{
			state = DA;
		}
		break;
		
		case AB:
		if(x > 600)
		{
			state = B;
		}
		else if (x < 400)
		{
			state = A;
		}
		break;
		
		case B:
		if(x > 600)
		{
			state = BC;
		}
		else if (x < 400)
		{
			state = AB;
		}
		break;
		
		case BC:
		if(x > 600)
		{
			state = C;
		}
		else if (x < 400)
		{
			state = B;
		}
		break;
		
		case C:
		if(x > 600)
		{
			state = CD;
		}
		else if (x < 400)
		{
			state = BC;
		}
		break;
		
		case CD:
		if(x > 600)
		{
			state = D;
		}
		else if (x < 400)
		{
			state = C;
		}
		break;
		
		case D:
		if(x > 600)
		{
			state = DA;
		}
		else if (x < 400)
		{
			state = CD;
		}
		break;
		
		case DA:
		if(x > 600)
		{
			state = A;
		}
		else if (x < 400)
		{
			state = D;
		}
		break;
		
		default:
		break;
	}
	switch(state)
	{
		case A:
		PORTD = 0x01;
		break;
		
		case AB:
		PORTD = 0x03;
		break;
		
		case B:
		PORTD = 0x02;
		break;
		
		case BC:
		PORTD = 0x06;
		break;
		
		case C:
		PORTD = 0x04;
		break;
		
		case CD:
		PORTD = 0x0C;
		break;
		
		case D:
		PORTD = 0x08;
		break;
		
		case DA:
		PORTD = 0x09;
		break;
		
		default:
		break;
	}
}

void LedSecTask()
{
	LEDS_Init();
	for(;;)
	{
		LEDS_Tick();
		vTaskDelay(3);
	}
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRD = 0xFF; PORTD = 0x00;
	
	InitADC();
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}