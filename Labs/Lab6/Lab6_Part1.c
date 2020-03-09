/*
 * Lab6_Part1.c
 *
 * Created: 10/26/2017 7:13:52 PM
 *  Author: student
 */ 


#include <avr/io.h>

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


int main(void)
{
	//DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
		
	unsigned char C = 0x00;
	unsigned char D = 0x00; 
	InitADC();
	uint16_t y;
	
	y = readadc(1);
	
	
    while(1)
    {
		y = readadc(1);
		
		C = (char) y;
		D = (char) (y>>8);
		
		PORTC = C;
 		PORTD = D; 
        //TODO:: Please write your application code 
    }
}