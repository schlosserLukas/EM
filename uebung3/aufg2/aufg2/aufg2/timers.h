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
	void(*callback)();
	uint8_t timerActive; //0 = inactive, 1 = init, 2 = active
} Timer;

volatile Timer timers[timerCount] = {0};

void declareTimer(TimerType, int, void(*callback)());
void startTimer(TimerType);
void cancelTimer(TimerType);


/*-----------------------TIMER-----------------*/

void timerInit(){
	TCNT0 = 6; //timer
	TCCR1B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));
	TCCR1B |= (1<<CS01);
	TIMSK1 |= (1<<TOIE1);
	sei();
}

void declareTimer(TimerType t, int time, void(*callback)())
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

ISR(TIMER0_OVF_vect){
	static uint16_t counter = 0;
	TCNT0 = 6;
	counter++;
	if (counter==20){
		for(int i = 0; i < timerCount; i++)
		{
			if(((GetSysTime() - timers[i].initTime) >= timers[i].time) && timers[i].timerActive == 2)
			{
				setEvent(timers[i].ev);
				cancelTimer(timers[i].timer);
			}
		}
		counter=0;
	}
}

#endif /* TIMERS_H_ */