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

#define F_CPU 8000000UL
#include <util/delay.h>
//#include "LCD_16x2_H_file.h"	/* Include LCD header file */

#define  Trigger_pin	PC7	/* Trigger pin */

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

enum sensorState {Sense} sensor_state;

char string[10];
unsigned long count;
double distance;
int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}

void Sensor_Init()
{
	sensor_state = Sense;
}

void Sensor_Tick()
{
	switch (sensor_state)
	{
		case Sense:
		break;
	}
	
	switch (sensor_state)
	{
		case Sense:
		/* Give 10us trigger pulse on trig. pin to HC-SR04 */
		PORTC |= (1 << Trigger_pin);
		_delay_us(10);
		PORTC &= (~(1 << Trigger_pin));
		
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x41;	/* Capture on rising edge, No prescaler*/
		TIFR1 = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR1 = 1<<TOV1;	/* Clear Timer Overflow flag */

		/*Calculate width of Echo by Input Capture (ICP) */
		
		while ((TIFR1 & (1 << ICF1)) == 0);/* Wait for rising edge */
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x01;	/* Capture on falling edge, No prescaler */
		TIFR1 = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR1 = 1<<TOV1;	/* Clear Timer Overflow flag */
		TimerOverflow = 0;/* Clear Timer overflow count */

		while ((TIFR1 & (1 << ICF1)) == 0);/* Wait for falling edge */
		count = ICR1 + (65535 * TimerOverflow);	/* Take count */
		distance = (double)count / 466.47;
		
		if (distance < 1)
		{
			PORTC = 0x40;
		}
		else if (distance > 1)
		{
			PORTC = 0x00;
		}

		// 		dtostrf(distance, 2, 2, string);/* distance to string */
		// 		strcat(string, " cm   ");	/* Concat unit i.e.cm */
		// 		LCD_String_xy(2, 0, "Dist = ");
		// 		LCD_String_xy(2, 7, string);	/* Print distance */
		_delay_ms(200);
		break;
	}
}

void SensorSecTask()
{
	Sensor_Init();
	for(;;)
	{
		Sensor_Tick();
		vTaskDelay(5);
	}
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(SensorSecTask, (signed portCHAR *)"SensorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;		/* Make trigger pin as output */
	DDRD = 0x00; PORTD = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; 		/* Turn on Pull-up */
	DDRC = 0xFF; PORTC = 0x00;
	
	// 	LCD_Init();
	// 	LCD_String_xy(1, 0, "Ultrasonic");
	
	sei();			/* Enable global interrupt */
	TIMSK1 = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;		/* Set all bit to zero Normal operation */
	
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
	
	
}