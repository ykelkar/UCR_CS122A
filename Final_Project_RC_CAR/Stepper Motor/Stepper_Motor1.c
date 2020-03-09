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
enum BtnState {Init, Turn_Right, Turn_Left, Rotation, Reset} btn_state;

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

void Btn_Init()
{
	btn_state = Init;
}

unsigned short numPhases;
unsigned char ccFlag = 3;

void Btn_Tick()
{
	
	x = readadc(0);
	switch(btn_state)
	{
		case Init:
		if (x > 600)
		{
			btn_state = Turn_Right;
		}
		else if (x < 400)
		{
			btn_state = Turn_Left;
		}
		break;
		
		case Turn_Right:
		if (x > 600)
		{
			btn_state = Turn_Right;
		}
		btn_state = Reset;
		break;
		
		case Turn_Left:
		if (x < 400)
		{
			btn_state = Turn_Right;
		}
		btn_state = Reset;
		break;
		
		case Reset:
		btn_state = Rotation;
		break;
		
		case Rotation:
		btn_state = Init;
		break;
		
		default:
		break;
	}
	switch(btn_state)
	{
		case Init:
		break;
		
		case Turn_Right:
		ccFlag = 1;
		numPhases = (20/5.625) * 64;
		break;
		
		case Turn_Left:
		ccFlag = 0;
		numPhases = (20/5.625) * 64;
		break;
		
		case Reset:
		if (ccFlag == 1)
		{
			ccFlag = 0;
			numPhases = (20/5.625) * 64;
		}
		else if (ccFlag == 0)
		{
			ccFlag = 1;
			numPhases = (20/5.625) * 64;
		}
		break;
		
		case Rotation:
		break;
		
		default:
		break;
	}
}


void BtnSecTask()
{
	Btn_Init();
	for(;;)
	{
		Btn_Tick();
		vTaskDelay(500);
	}
}


void LEDS_Init()
{
	state = A;
}

unsigned short counter = 0;

void LEDS_Tick()
{
	
	switch(state)
	{
		case A:
		if(ccFlag == 1)
		{
			if(counter != numPhases)
			{
				counter++;
				state = AB;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			
			if(counter != numPhases)
			{
				counter++;
				state = DA;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		break;
		
		case AB:
		if(ccFlag == 1)
		{
			if(counter != numPhases)
			{
				counter++;
				state = B;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			if(counter != numPhases)
			{
				counter++;
				state = A;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		break;
		
		case B:
		if(ccFlag == 1)
		{
			if (counter != numPhases)
			{
				counter++;
				state = BC;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			if (counter != numPhases)
			{
				counter++;
				state = AB;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		break;
		
		case BC:
		if(ccFlag == 1)
		{
			if (counter != numPhases)
			{
				counter++;
				state = C;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			if (counter != numPhases)
			{
				counter++;
				state = B;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		break;
		
		case C:
		if(ccFlag == 1)
		{
			if (counter != numPhases)
			{
				counter++;
				state = CD;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			if (counter != numPhases)
			{
				counter++;
				state = BC;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		break;
		
		case CD:
		if(ccFlag == 1)
		{
			if (counter != numPhases)
			{
				counter++;
				state = D;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			if (counter != numPhases)
			{
				counter++;
				state = C;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		break;
		
		case D:
		if(ccFlag == 1)
		{
			if (counter != numPhases)
			{
				counter++;
				state = DA;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			if(counter != numPhases)
			{
				counter++;
				state = CD;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		break;
		
		case DA:
		if(ccFlag == 1)
		{
			if (counter != numPhases)
			{
				counter++;
				state = A;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
		}
		else if (ccFlag == 0)
		{
			if (counter != numPhases)
			{
				counter++;
				state = D;
			}
			else
			{
				counter = 0;
				numPhases = 0;
			}
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
	xTaskCreate(BtnSecTask, (signed portCHAR *)"BtnSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRD = 0xFF; PORTD=0x00;
	DDRB = 0x00; PORTB = 0xFF;
	
	state = Init;

	InitADC();
	
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}