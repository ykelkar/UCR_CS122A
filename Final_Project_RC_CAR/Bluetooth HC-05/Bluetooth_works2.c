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

enum bluetoothState {Start, Receive} bluetooth_state;
unsigned char bluetooth_val = 0x00;

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
		case Receive:
		if(USART_HasReceived(0))
		{
			bluetooth_val = USART_Receive(0);
			USART_Flush(0);
			PORTA = bluetooth_val;
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
		vTaskDelay(100);
	}
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(BluetoothSecTask, (signed portCHAR *)"BluetoothSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRA = 0xFF;PORTA = 0x00;
	initUSART(0);
	USART_Flush(0);

	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();

	// 	TimerSet(100);
	// 	TimerOn();
	//
	// 	state = Start;
	// 	while(1)
	// 	{
	// 		Tick();
	//
	// 		while(!TimerFlag);
	// 		TimerFlag = 0;
	// 	}
}