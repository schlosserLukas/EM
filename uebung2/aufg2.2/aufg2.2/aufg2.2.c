/*
 * aufg2.c
 *
 * Created: 13.04.2023 09:18:02
 *  Author: scl36281
 */ 



#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>


volatile uint8_t running = 1;
volatile uint8_t counter = 7;
volatile uint8_t startValue = 0;


uint8_t setBit(uint8_t value, uint8_t index, uint8_t var)
{
	
	var= (var &~(1UL <<index)) | (value << index); //starts counting at position 0
	return var;
}

uint8_t getBit(uint8_t var,uint8_t index)
{
	return((var >> index) & 1);
}

void setLEDs(uint8_t counter){
	if(getBit(counter, 2)){
		PORTD = setBit(0,PORTD0,PORTD);
	}
	else{
		PORTD|=(1<<PORTD0);
	}
	if(getBit(counter,1)){
		PORTD = setBit(0, PORTD1, PORTD);
	}
	else{
		PORTD|=(1<<PORTD1);
	}
	if(getBit(counter,0)){
		PORTD = setBit(0, PORTD6, PORTD);
	}
	else{
		PORTD|=(1<<PORTD6);
	}
	
	
}

uint8_t setStartValue(){
	uint8_t startValue=0;
	uint8_t a5Val = !getBit(PIND, PIND5);
	uint8_t a6Val = !getBit(PIND, PIND3);
	uint8_t a7Val = !getBit(PIND, PIND4);
	startValue=setBit(a5Val,0,startValue);
	startValue=setBit(a6Val,1,startValue);
	startValue=setBit(a7Val,2,startValue);
	return startValue;
}





ISR(INT0_vect)
{
	running = 1;
	counter = startValue;
	
}

ISR(PCINT1_vect)
{
	running = 0;
	counter = startValue;
	setLEDs(counter);
	//PORTD ^= (1<<PORTD7);
	
}

ISR(PCINT0_vect)
{
	counter = startValue;
	PORTD ^= (1<<PORTD7);
	
}


int main(void)
{

	DDRD|=(1<<DDD0);//LED A1
	DDRD|=(1<<DDD1);//LED A4
	DDRD|=(1<<DDD6);//LED A7
	DDRD |= (1<<DDD7); //LED A3
	DDRB = setBit(1, PORTB0, DDRB); // output for sw interupt
	
	PORTD|=(1<<DDD5);//pullup widerstand 0
	PORTD|=(1<<DDD3);//pullup widerstand 1
	PORTD|=(1<<DDD4);//pullup widerstand 2
	PORTC|=(1<<DDC3);//pullup widerstand 3 fuer Taster a3
	PORTD|=(1<<DDD2);//pullup widerstand 4 fuer Taster a4
	PORTD|=(1<<PORTD0);
	PORTD|=(1<<PORTD1),
	PORTD|=(1<<PORTD6);
	
	// activate Interrupt
	EICRA &= ~((1<<ISC00)|(1<<ISC01));
	EICRA |= (1<<ISC01);
	EIMSK |= (1<<INT0);
	PCICR = setBit(1, PCIE1, PCICR);
	PCICR = setBit(1, PCIE0, PCICR);
	PCMSK1 = setBit(1,PCINT11,PCMSK1);
	
	PCMSK0 = setBit(1,PCINT0, PCMSK0);
	
	sei();
	
	startValue = setStartValue();
	counter = startValue;
	
	
	while(1)
	{
		startValue = setStartValue();
		if(running){
			setLEDs(counter);
			if(counter == 0){
				//counter = startValue
				//sw interrupt
				PORTB^= 1<<PORTB0;
			}
			else{
				counter--;
			}
		}
		_delay_ms(1000);
	}
}