#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t running = 1;
volatile uint8_t startValue = 0;
volatile uint8_t counter = 7;

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
	uint8_t a5Val = !getBit(PIND, PIND5);
	uint8_t a6Val = !getBit(PIND, PIND6);
	uint8_t a7Val = !getBit(PIND, PIND7);
	startValue=setBit(a5Val,0,startValue);
	startValue=setBit(a6Val,1,startValue);
	startValue=setBit(a7Val,2,startValue);
	return startValue;
}

ISR(INT0_vect){
	running=1;
	counter = startValue;
}

ISR(PCINT1_vect){
	running=0;
	counter = startValue;
	setLEDs(counter);
}

ISR(PCINT2_vect){
	PORTB^=(1<<PORTB3);
	counter = startValue;
}
int main(void)
{
	DDRB|=(1<<DDB0);
	DDRB|=(1<<DDB1);
	DDRB|=(1<<DDB2);
	DDRB|=(1<<DDB3); //LED for debugging
	DDRD = setBit(1, PORTD4, DDRD); //set sw interrupt port -> outport
	PORTD|=(1<<DDD2); //pullup widerstand 0
	PORTC|=(1<<DDC3); //pullup widerstand 1
	PORTD|=(1<<DDD5); //pullup widerstand 2
	PORTD|=(1<<DDD6); //pullup widerstand 3
	PORTD|=(1<<DDD7); //pullup widerstand 4
	
	PORTB|=(1<<PORTB0); //led default off
	PORTB|=(1<<PORTB1);	//led default off
	PORTB|=(1<<PORTB2);	//led default off
	PORTB|=(0<<PORTB3);
	
	EICRA = setBit(0, ISC00, EICRA); //int0
	EICRA = setBit(1, ISC01, EICRA); //int 0
	EIMSK = setBit(1, INT0, EIMSK); //int 0
	PCICR = setBit(1, PCIE1, PCICR); //pcint1
	PCICR = setBit(1, PCIE2, PCICR); //pcint2 (sw interrupt)
	PCMSK1 = setBit(1, PCINT11, PCMSK1); //pcint1 / a4
	
	PCMSK2 = setBit(1, PCINT20, PCMSK2); //sw interrupt
	sei(); //enable global interrupt
	
	startValue = setStartValue();
	counter = startValue;
	while(1)
	{
		startValue = setStartValue();
		if(running){
			setLEDs(counter);
			if(counter == 0){
				//counter = startValue;
				//trigger sw interrupt
				PORTD^=1<<PORTD4;
				__asm__{
					nop
					nop
					nop
					nop
				}
				}else{
				counter--;
				
			}
		}

		_delay_ms(1000);
	}
}

