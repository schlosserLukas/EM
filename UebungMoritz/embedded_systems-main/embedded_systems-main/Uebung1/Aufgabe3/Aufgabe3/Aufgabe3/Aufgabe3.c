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
void setLEDs(uint8_t counter){
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
}

uint8_t setStartValue(){
	uint8_t startValue;
	uint8_t a5Val = !getBit(PIND, PIND2);
	uint8_t a6Val = !getBit(PIND, PIND3);
	uint8_t a7Val = !getBit(PIND, PIND4);
	startValue=setBit(a5Val,0,startValue);
	startValue=setBit(a6Val,1,startValue);
	startValue=setBit(a7Val,2,startValue);
	return startValue;
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
	PORTD|=(1<<DDD0); //pullup widerstand 0
	PORTD|=(1<<DDD1); //pullup widerstand 1
	PORTD|=(1<<DDD2); //pullup widerstand 2
	PORTD|=(1<<DDD3); //pullup widerstand 3
	PORTD|=(1<<DDD4); //pullup widerstand 4
	PORTB|=(1<<PORTB0);
	PORTB|=(1<<PORTB1);
	PORTB|=(1<<PORTB2);
	
	uint8_t delayCounter, a3Val, prevA3Val, a4Val, prevA4Val = 0, a5Val, a6Val, a7Val;
	uint8_t startValue =setStartValue();
	uint8_t running=1;
	while(1)
	{
		a3Val= getBit(PIND,PIND0);
		if(a3Val && !prevA3Val){
			running=0;
			counter = startValue;
			setLEDs(counter);
		}
		prevA3Val=a3Val;
		a4Val = getBit(PIND,PIND1);
		if(a4Val && !prevA4Val){
			running=1;
			counter = startValue;
		}
		prevA4Val=a4Val;
		startValue = setStartValue();
		if(running){
				if(delayCounter == 9 ){
					setLEDs(counter);
					if(counter == 0){
						counter = startValue;
						}else{
						counter--;
					}
					delayCounter=0;
					
					}else{
					delayCounter++;
				}
		}
		
		_delay_ms(100);
	}
}

