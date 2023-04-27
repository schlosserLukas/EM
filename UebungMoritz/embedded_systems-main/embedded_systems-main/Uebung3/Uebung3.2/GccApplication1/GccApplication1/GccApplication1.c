#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "events.h"

#define EVENT_50MS 0x01
#define EVENT_1000MS 0x02
#define TIMER_1_SEC Timer0
#define TIMER_50_MSEC Timer1
volatile uint8_t running = 1;
volatile uint8_t startValue = 0;
volatile uint8_t counter = 7;


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

ISR(PCINT2_vect){
	counter = startValue;
}

void toggleLED(){
	PORTB ^= (1<<PORTB3);
}

void buttonCheck(){
	setEvent(EVENT_50MS);
	//valueD2 = getBit(PIND,PIND2);
	//if( valueD2 && prevValueD2==0 ){
		////start
		//running=1;
		//counter = startValue;
		//
	//}
	//prevValueD2=valueD2;
	//
	//valueD3 = getBit(PIND,PIND3);
	//if( valueD3 && prevValueD3==0 ){
		////stop
		//running=0;
		//counter = startValue;
		//setLEDs(counter);
	//}
	//prevValueD3=valueD3;
	//startTimer(TIMER_50_MSEC);
}

void countDown(){
	setEvent(EVENT_1000MS);
	//startValue = setStartValue();
	//if(running){
		//setLEDs(counter);
		//if(counter == 0){
			////trigger sw interrupt
			//PORTD^=1<<PORTD4;
			////__asm__{
			////nop
			////nop
			////nop
			////nop
			////}
			//}else{
			//counter--;
			//
		//}
	//}
	//startTimer(TIMER_1_SEC);
}
int main(void)
{
	DDRB|=(1<<DDB0);
	DDRB|=(1<<DDB1);
	DDRB|=(1<<DDB2);
	DDRB|=(1<<DDB3); //LED for debugging
	DDRD = setBit(1, PORTD4, DDRD); //set sw interrupt port -> outport
	PORTC|=(1<<DDC3); //pullup widerstand 1
	PORTD|=(1<<DDD5); //pullup widerstand 2
	PORTD|=(1<<DDD6); //pullup widerstand 3
	PORTD|=(1<<DDD7); //pullup widerstand 4
	PORTD|=(1<<DDD2); //pullup
	PORTD|=(1<<DDD3); //pullup
	
	
	PORTB|=(1<<PORTB0); //led default off
	PORTB|=(1<<PORTB1);	//led default off
	PORTB|=(1<<PORTB2);	//led default off
	PORTB|=(1<<PORTB3);

	PCICR = setBit(1, PCIE2, PCICR); //pcint2 (sw interrupt)
	
	PCMSK2 = setBit(1, PCINT20, PCMSK2); //sw interrupt
	
	sei(); //enable global interrupt
	
	startValue = setStartValue();
	counter = startValue;
	uint8_t prevValueD2,valueD2,prevValueD3,valueD3;
	timerInit();
	declareTimer(TIMER_1_SEC,1000000,countDown);
	declareTimer(TIMER_50_MSEC, 50000,buttonCheck);
	
	startTimer(TIMER_1_SEC);
	startTimer(TIMER_50_MSEC);
	
	while(1)
	{
		if(eventIsSet(EVENT_50MS)){
			valueD2 = getBit(PIND,PIND2);
			if( valueD2 && prevValueD2==0 ){
				//start
				running=1;
				counter = startValue;
			
			}
			prevValueD2=valueD2;
			
			valueD3 = getBit(PIND,PIND3);
			if( valueD3 && prevValueD3==0 ){
				//stop
				running=0;
				counter = startValue;
				setLEDs(counter);
			}
			prevValueD3=valueD3;
			clearEvent(EVENT_50MS);
			startTimer(TIMER_50_MSEC);
		}
		
		if(eventIsSet(EVENT_1000MS)){
			toggleLED();
			startValue = setStartValue();
			if(running){
				setLEDs(counter);
				if(counter == 0){
					//trigger sw interrupt
					PORTD^=1<<PORTD4;
					//__asm__{
					//nop
					//nop
					//nop
					//nop
					//}
					}else{
					counter--;
					
				}
			}
			clearEvent(EVENT_1000MS);
			startTimer(TIMER_1_SEC);
		}
		_delay_ms(10);
	}
}
