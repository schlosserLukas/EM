/*
* Aufgabe1.c
*
* Created: 23.03.2023 08:46:16
*  Author: ham35274
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

uint8_t setBit(uint8_t value, uint8_t index, uint8_t var)
{
	var = (var & ~(1UL << index)) | (value << index); // starts counting at position 0
	return var;
}

uint8_t getBit(uint8_t var, uint8_t index)
{
	return ((var >> index) & 1);
}

int main(void)
{
	uint8_t counter = 7;
	counter|=1;
	counter|=1<<1;
	counter |= 1<<2;
	DDRB|=(1<<DDB0);
	DDRB|=(1<<DDB1);
	DDRB|=(1<<DDB2);
	PORTB|=(0<<PORTB0);
	PORTB|=(1<<PORTB1);
	PORTB|=(1<<PORTB2);
	while(1)
	{
		if(getBit(counter,2)){
			PORTB=setBit(0,PORTB0,PORTB);
		}
		else{
			PORTB|=(1<<PORTB0);
		}
		if(getBit(counter,1)){
			PORTB=setBit(0,PORTB1,PORTB);
		}
		else{
			PORTB|=(1<<PORTB1);
		}
		if(getBit(counter,0)){
			PORTB=setBit(0,PORTB2,PORTB);
		}
		else{
			PORTB|=(1<<PORTB2);
		}
		
		if(counter == 0){
			counter = 7;
		}else{
				counter--;
		}
	
		_delay_ms(1000);
	}
}
