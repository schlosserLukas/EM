/*
 * aufg2.c
 *
 * Created: 20.04.2023 09:27:04
 *  Author: scl36281
 */ 




#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "events.h"

#define EVENT_0 0x01
#define EVENT_1 0x02

#define timerCount 5
#define Timer0 (uint8_t)0
#define Timer1 (uint8_t)1
#define Timer2 (uint8_t)2
#define Timer3 (uint8_t)3
#define Timer4 (uint8_t)4

volatile uint8_t running = 1;
volatile uint8_t counter = 7;
volatile uint8_t startValue = 0;

typedef uint8_t EventType;
typedef uint8_t TimerType;
typedef struct Timer
{
	TimerType timer;
	uint32_t initTime;
	int time;
	EventType ev;
	uint8_t timerActive; //0 = inactive, 1 = init, 2 = active
} Timer;

volatile Timer timers[timerCount] = {0};

void setTimer(TimerType, int, EventType);
void startTimer(TimerType);
void cancelTimer(TimerType);


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




ISR(TIMER0_OVF_vect){
	static uint16_t counter = 0;
	TCNT0 = 6;
	counter++;
	if (counter%50==0){
		setEvent(EVENT_0);
	}
	if(counter==1000){
		setEvent(EVENT_1);
		counter = 0;
	}
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
	PORTB|=(1<<DDB0);//pullup widerstand 3 fuer Taster a3
	PORTD|=(1<<DDD2);//pullup widerstand 4 fuer Taster a4
	PORTD|=(1<<PORTD0);
	PORTD|=(1<<PORTD1),
	PORTD|=(1<<PORTD6);
	
	// activate Interrupt
	
	DDRB |= (1<<PORTB1)|(1<<PORTB0);
	TCNT0 = 6;
	TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));
	TCCR0B |= (1<<CS01)|(1<<CS00);
	TIMSK0 |= (1<<TOIE0);
	sei();
	
	startValue = setStartValue();
	counter = startValue;
	uint8_t a3Val = 0, prevA3Val, a4Val = 0, prevA4Val;
	
	while(1)
	{
		startValue = setStartValue();
		
		if(eventIsSet(EVENT_1)){
			clearEvent(EVENT_1);
			if(running){
				setLEDs(counter);
				if(counter==0){
					counter = startValue;
				}else{
					counter--;
				}
			}
		}
		
		if(eventIsSet(EVENT_0)){
			clearEvent(EVENT_0);
			PORTB ^= (1<<PORTB1);
			a3Val = getBit(PINB, PINB0);
			if(a3Val && !prevA3Val){
				running = 0;
				counter = startValue;
				setLEDs(counter);
			}
			prevA3Val = a3Val;
			
			a4Val = getBit(PIND, PIND2);
			if(a4Val && !prevA4Val){
				running = 1;
				counter = startValue;
				
			}
			prevA4Val = a4Val;
		}
		_delay_ms(10);
	}
}

/*-----------------------TIMER-----------------*/

void setTimer(TimerType t, int time, EventType ev)
{
	for(int i = 0; i < timerCount; i++){
		if(timers[i].timerActive == 0){
			timers[i].timer = t;
			timers[i].timerActive = 1;
			timers[i].ev = ev;
			timers[i].initTime = 0;
			timers[i].time = time;
			break;
		}
	}
}

void startTimer(TimerType t)
{
	for(int i = 0; i < timerCount; i++)
	{
		if(timers[i].timer == t)
		{
			timers[i].timerActive = 2;
			timers[i].initTime = GetSysTime();
			break;
		}
	}
}

void cancelTimer(TimerType t)
{
	for(int i = 0; i < timerCount; i++)
	{
		if(timers[i].timer == t)
		{
			timers[i].timerActive = 0;
		}
	}
}

