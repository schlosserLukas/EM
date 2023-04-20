#ifndef TIMERS_H_
#define TIMERS_H_

#define timerCount 5
#define Timer0 (uint8_t)0
#define Timer1 (uint8_t)1
#define Timer2 (uint8_t)2
#define Timer3 (uint8_t)3
#define Timer4 (uint8_t)4

typedef uint8_t TimerType;
typedef struct Timer
{
	TimerType timer;
	uint32_t time; //time in µs
	uint64_t initTime;
	void(*callback)();
	uint8_t timerActive; //0 = inactive, 1 = init, 2 = active
} Timer;

volatile Timer timers[timerCount] = {0};

void declareTimer(TimerType, int, void(*callback)());
void startTimer(TimerType);
void cancelTimer(TimerType);

uint64_t counter = 0;
/*-----------------------TIMER-----------------*/

void timerInit(){
	TCNT1 = 0; //timer init value
	TCCR1B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));
	TCCR1B |= (1<<CS01); //prescaler8, alle 0,5µs tick
	TIMSK1 |= (1<<TOIE1);
	sei();
}

void declareTimer(TimerType t, int time, void(*callback)())
{
	for(int i = 0; i < timerCount; i++){
		if(timers[i].timerActive == 0){
			timers[i].timer = t;
			timers[i].timerActive = 1;
			timers[i].callback = callback;
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

ISR(TIMER0_OVF_vect){
	static uint8_t isrCounter = 0;
	TCNT1 = 0;
	isrCounter++;
	if (isrCounter%2==0){ //alle 1µs
		for(int i = 0; i < timerCount; i++)
		{
			if(((counter - timers[i].initTime) >= timers[i].time) && timers[i].timerActive == 2)
			{
				setEvent(timers[i].ev);
				cancelTimer(timers[i].timer);
			}
		}
		isrCounter=0;
		counter++;
	}
}

#endif /* TIMERS_H_ */