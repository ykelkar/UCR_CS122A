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
#include "LCD_16x2_H_file.h"	/* Include LCD header file */

#define  Trigger_pin	PC7	/* Trigger pin */

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

enum stepperstate {A, AB, B, BC, C, CD, D, DA} state;
enum joystickState {Init, Turn_Right, Turn_Left, Reset, Wait} joystick_state;
enum States {Init1, Wait1, Backward, Forward, Halt} motor_state;
enum sensorState {Sense} sensor_state;

unsigned long count;
double distance;
int TimerOverflow = 0;

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
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

uint16_t x, y;

void Motor_Init()
{
	motor_state = Init1;
}

void Motor_Tick()
{
	y = readadc(1);
	switch(motor_state)
	{
		case Init1:
		motor_state = Wait1;
		break;
		
		case Wait1:
		if (y > 600)
		{
			motor_state = Forward;
		}
		else if (y < 400)
		{
			motor_state = Backward;
		}
		else
		{
			motor_state = Halt;
		}
		break;
		
		case Backward:
		motor_state = Wait1;
		break;
		
		case Forward:
		motor_state = Wait1;
		break;
		
		case Halt:
		motor_state = Wait1;
		break;
		
		default:
		break;
	}
	switch(motor_state)
	{
		case Init1:
		break;
		
		case Wait1:
		//PORTC = 0b00001100;
		break;
		
		case Forward:
		// counter-clockwise rotation
		PORTC = 0b00000110;     // PC0 = Low = 0
		// PC1 = High = Vcc
		// PC2 = Low = 0
		// PC3 = High = Vcc
		break;
		
		case Backward:
		// clockwise rotation
		PORTC = 0b00001001;     // PC0 = High = Vcc
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

void MotorSecTask()
{
	Motor_Init();
	for(;;)
	{
		Motor_Tick();
		vTaskDelay(1);
	}
}

void Joystick_Init()
{
	joystick_state = Init;
}

unsigned short numPhases;
unsigned char ccFlag = 3;

void Joystick_Tick()
{
	
	x = readadc(0);
	switch(joystick_state)
	{
		case Init:
		if (x > 600)
		{
			joystick_state = Turn_Right;
		}
		else if (x < 400)
		{
			joystick_state = Turn_Left;
		}
		break;
		
		case Turn_Right:
		joystick_state = Wait;
		break;
		
		case Turn_Left:
		joystick_state = Wait;
		break;
		
		case Wait:
		if ((x > 400) && (x < 600))
		{
			joystick_state = Reset;
		}
		else
		{
			joystick_state = Wait;
		}
		break;
		
		case Reset:
		joystick_state = Init;
		break;
		
		default:
		break;
	}
	switch(joystick_state)
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
		
		case Wait:
		break;
		
		default:
		break;
	}
}


void JoystickSecTask()
{
	Joystick_Init();
	for(;;)
	{
		Joystick_Tick();
		vTaskDelay(500);
	}
}


void Stepper_Init()
{
	state = A;
}

unsigned short counter = 0;

void Stepper_Tick()
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

void StepperSecTask()
{
	Stepper_Init();
	for(;;)
	{
		Stepper_Tick();
		vTaskDelay(3);
	}
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
		
		if (distance > 10)
		{
			PORTC = 0x40;
		}
		else if (distance < 10)
		{
			PORTC = 0x00;
		}

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
	xTaskCreate(MotorSecTask, (signed portCHAR *)"MotorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(JoystickSecTask, (signed portCHAR *)"JoystickSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(StepperSecTask, (signed portCHAR *)"StepperSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	xTaskCreate(SensorSecTask, (signed portCHAR *)"SensorSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
	
}

int main(void)
{
	DDRD = 0x00; PORTD = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; 		/* Turn on Pull-up */
	DDRC = 0xFF; PORTC = 0x00;
	
	sei();			/* Enable global interrupt */
	TIMSK1 = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;		/* Set all bit to zero Normal operation */

	InitADC();
	
	//Start Tasks
	StartSecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}