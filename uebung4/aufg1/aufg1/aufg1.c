/*
 * aufg1.c
 *
 * Created: 27.04.2023 09:24:42
 *  Author: scl36281
 */ 

#define F_CPU 16000000UL

#define BAUDRATE 9600
#define BAUD_CONST (((F_CPU/(BAUDRATE*16UL)))-1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BUFFERSIZE 32

volatile int8_t readBuffer[BUFFERSIZE]; 
volatile uint8_t bufferLength = 0;
volatile uint8_t readIndex = 0, writeIndex = 0;

int8_t readFromReadBuffer(){
	if(bufferLength > 0){
		int8_t val = readBuffer[readIndex];
		readBuffer[readIndex] = -1;
		readIndex = (readIndex + 1) % BUFFERSIZE;
		bufferLength--;
		return val;
	}
	return -1;
}

void writeToReadBuffer(int8_t val){
	readBuffer[writeIndex] = val;
	writeIndex = (writeIndex + 1) % BUFFERSIZE;
	bufferLength++;
	return;
}

uint8_t getBit(uint8_t var, uint8_t index)
{
	return ((var >> index) & 1);
}

void USART_Init(){
	UBRR0H = (BAUD_CONST >> 8);
	UBRR0L = BAUD_CONST;
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
}


void USART_Transmit(unsigned char data){
	while(!(UCSR0A & (1<<UDRE0))) ;
	UDR0 = data;
}


unsigned char USART_Receive(){
	while(!(UCSR0A & (1<<RXC0))) ;
	return UDR0;
}

ISR(USART_RX_vect){
	if(!(getBit(UCSR0A, FE0)||getBit(UCSR0A, DOR0)||getBit(UCSR0A, UPE0))){
		uint8_t UDR0_temp = UDR0;
		writeToReadBuffer(UDR0_temp);
	}
}

int main(void)
{
	UCSR0B |= (1<<RXCIE0);
	sei();
	char name[10];
	int g = 0;
	const char meldung[]="Hier ATmega. Wer da?";
	USART_Init();
	for(int f=0;meldung[f]!='\0';f++){
		USART_Transmit(meldung[f]);
	}
	int8_t readValue = -1;
    while (1) 
    {
		//name[g] = USART_Receive();
		//if (name[g]!=0x0d){
			//USART_Transmit(name[g]);
			//g++;
			//continue;	
		//}
		//USART_Transmit(0x0d);
		//USART_Transmit('H');
		//USART_Transmit('i');
		//USART_Transmit(' ');
		//for(int f=0;f<g;f++){
			//USART_Transmit(name[f]-32);
		//}
		//g = 0;
		//USART_Transmit(0x0d);	
		readValue = readFromReadBuffer();
		if(readValue > -1){
			USART_Transmit(readValue);
		}
		_delay_ms(300);
    }
}

