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

#include "C:\Users\Avanti\Documents\Atmel Studio\7.0\GccApplication8\GccApplication8\includes\usart_ATmega1284.h"

enum LEDState {A, AB, B, BC, C, CD, D, DA} state;
enum bluetoothState {Start, Receive, Send_Val} bluetooth_state;

unsigned char bluetooth_val;


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
		if(bluetooth_val == 'R')
		{
			state = AB;
		}
		else if (bluetooth_val == 'L')
		{
			state = DA;
		}
		break;
		
		case AB:
		if(bluetooth_val == 'R')
		{
			state = B;
		}
		else if (bluetooth_val == 'L')
		{
			state = A;
		}
		break;
		
		case B:
		if(bluetooth_val == 'R')
		{
			state = BC;
		}
		else if (bluetooth_val == 'L')
		{
			state = AB;
		}
		break;
		
		case BC:
		if(bluetooth_val == 'R')
		{
			state = C;
		}
		else if (bluetooth_val == 'L')
		{
			state = B;
		}
		break;
		
		case C:
		if(bluetooth_val == 'R')
		{
			state = CD;
		}
		else if (bluetooth_val == 'L')
		{
			state = BC;
		}
		break;
		
		case CD:
		if(bluetooth_val == 'R')
		{
			state = D;
		}
		else if (bluetooth_val == 'L')
		{
			state = C;
		}
		break;
		
		case D:
		if(bluetooth_val == 'R')
		{
			state = DA;
		}
		else if (bluetooth_val == 'L')
		{
			state = CD;
		}
		break;
		
		case DA:
		if(bluetooth_val == 'R')
		{
			state = A;
		}
		else if (bluetooth_val == 'L')
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
		PORTB = 0x01;
		break;
		
		case AB:
		PORTB = 0x03;
		break;
		
		case B:
		PORTB = 0x02;
		break;
		
		case BC:
		PORTB = 0x06;
		break;
		
		case C:
		PORTB = 0x04;
		break;
		
		case CD:
		PORTB = 0x0C;
		break;
		
		case D:
		PORTB = 0x08;
		break;
		
		case DA:
		PORTB = 0x09;
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

void Bluetooth_Init()
{
	bluetooth_state = Start;
}

void Bluetooth_Tick()
{
	switch (bluetooth_state)
	{
		
		case Start:
		bluetooth_state = Receive;
		break;
		
		case Receive:
		bluetooth_state = Receive;
		break;
		
		default:
		break;
	}
	
	switch (bluetooth_state)
	{
		case Start:
		bluetooth_val = 0;
		break;
		
		case Receive:
		if(USART_HasReceived(0))
		{
			bluetooth_val = USART_Receive(0);
			USART_Flush(0);
			PORTA = bluetooth_val;
			// 			porta = 1;
		}
		break;
		
		default:
		break;
	}
}

void BluetoothSecTask()
{
	Bluetooth_Init();
	for(;;)
	{
		Bluetooth_Tick();
		vTaskDelay(1);
	}
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(BluetoothSecTask, (signed portCHAR *)"BluetoothSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRB = 0xFF; PORTB=0x00;
	//DDRB = 0x00; PORTB = 0xFF;
	
	InitADC();
	
	initUSART(0);
	USART_Flush(0);
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}