#ifndef TIMERS_H_INCLUDED
#define TIMERS_H_INCLUDED
#include <stdbool.h>
#define timerCount 5

// start at 0, end with 4
#define Timer0 (uint8_t)0
#define Timer1 (uint8_t)1
#define Timer2 (uint8_t)2
#define Timer3 (uint8_t)3
#define Timer4 (uint8_t)4

typedef uint8_t TimerType;
typedef struct Timer
{
	TimerType timer;
	uint32_t time;	//time in 탎, at least 100 탎
	void(*callback)();
	uint8_t timerActive;	//0 = inactive, 1 = init, 2 = active
	uint64_t initTime;	//systime
}Timer;
volatile Timer timers[timerCount] = { 0 };
volatile uint64_t systimeCounter = 0;

uint8_t setBit(uint8_t value, uint8_t index, uint8_t var)
{
	var = (var & ~(1UL << index)) | (value << index); // starts counting at position 0
	return var;
}

uint8_t getBit(uint8_t var, uint8_t index)
{
	return ((var >> index) & 1);
}
void declareTimer(TimerType t, uint32_t time, void(*callback)())
{
	for (int i = 0; i < timerCount; i++)
	{
		if (timers[i].timerActive == 0)
		{
			unsigned char sreg_old = SREG;	//critical
			cli();
			timers[i].timer = t;
			timers[i].timerActive = 1;
			timers[i].callback = callback;
			timers[i].initTime = 0;
			timers[i].time = time/100; //convert time into 100 탎 steps
			SREG = sreg_old;	//leave critical
			break;
		}
	}
}

void startTimer(TimerType t)
{
	for (int i = 0; i < timerCount; i++)
	{
		if (timers[i].timer == t)
		{
			unsigned char sreg_old = SREG;	//critical
			cli();
			timers[i].timerActive = 2;
			timers[i].initTime = systimeCounter;
			SREG = sreg_old;	//leave critical
			break;
		}
	}
}

void cancelTimer(TimerType t)
{
	for (int i = 0; i < timerCount; i++)
	{
		if (timers[i].timer == t)
		{
			unsigned char sreg_old = SREG;	//critical
			cli();
			timers[i].timerActive = 0;
			SREG = sreg_old;	//leave critical
		}
	}
}

void timerInit()
{
	TCNT1 = 65336;	//timer init value
	TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
	TCCR1B |= (1 << CS11);	//prescaler 8, alle 0,5탎 tick
	TIMSK1 |= (1 << TOIE1);
	sei();
}

ISR(TIMER1_OVF_vect)
{

	TCNT1 = 65336;
	for (int i = 0; i < timerCount; i++)
	{
		if (((systimeCounter - timers[i].initTime) >= timers[i].time) && timers[i].timerActive == 2)
		{
			cancelTimer(timers[i].timer);
			timers[i].callback();
		}
	}
	systimeCounter++;
}



#endif /*TIMERS_H_INCLUDED */