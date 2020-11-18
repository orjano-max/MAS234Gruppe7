/*
 * SoftBlink.cpp
 *
 * Created: 23.10.2020 11:15:16
 * Author : kolaj
 */ 


#define F_CPU 1000000UL // 1 MHz clock speed

#include <avr/io.h>
#include <util/delay.h>

float dutyCycle = 0;

int main(void)
{
	
	TCCR1A |= (1<<WGM10) | (1<<COM1A1);
	TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10);
	DDRB |= (1<<PB1);
		
	dutyCycle = 100; 
	
	OCR1A = (dutyCycle/100)*255;
	
}

