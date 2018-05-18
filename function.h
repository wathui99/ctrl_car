#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DDR_MOTOR_L DDRD
#define DDR_MOTOR_R DDRB
#define DDR_SERVO DDRD

#define PORT_MOTOR_L PORTD
#define PORT_MOTOR_R PORTB
#define PORT_SERVO PORTD
#define MOTOR_L 4
#define MOTOR_R 3
#define SERVO 5

#define DIR00	0
#define DIR01	1
#define DIR10	3
#define DIR11   6

#define true 1
#define false 0
#define SERVO_CENTER		2550	//Sai số của cần sensor trên xe (am se dich qua trai, duong se dich qua phai)
#define ANGLE_MAX           150
#define STEP				5


//===================BUTTON + SWITCH=====================
uint8_t get_button(uint8_t keyid)
{
	if ( (PINB & 0x0e) != 0x0e)
	{
		_delay_ms(100);
		if ((PINB|keyid) == keyid) return 1;
	}
	return 0;
}

//================RATIO + SERVO + MOTOR ================
void speed(float left, float right, float persent) { //max left la 255, max right la 255
	left*=(persent/100);
	right*=(persent/100);
	if (left>=0) {
		sbi(PORTD,DIR10);
		cbi(PORTD,DIR11);
		if (left*157>40000) OCR1B=40000;
		else
			OCR1B=(int)left*157;
	}
	else {
		cbi(PORTD,DIR10);
		sbi(PORTD,DIR11);
		if (left*157<-(40000)) OCR1B=40000;
		else
			OCR1B=(int)-left*157;
	}
	if (right>=0) {
		sbi(PORTD,DIR00);
		cbi(PORTD,DIR01);
		if(right>255) right=255;
		OCR2=(int)right;
	}
	else {
		cbi(PORTD,DIR00);
		sbi(PORTD,DIR01);
		if(right<-255) right=-255;
		OCR2=-(int)right;
	}
}
void handle(float goc)
{
	if (goc>ANGLE_MAX) goc=ANGLE_MAX;
	if (goc<-ANGLE_MAX) goc=-ANGLE_MAX;
	OCR1A=SERVO_CENTER+(int)goc*STEP; //duong phai, am trai
									//45* =1000;
}

uint16_t adc_read(uint8_t ch)
{
	ADMUX = (1<< REFS0)|ch;									// selecting channel
	ADCSRA|=(1<<ADSC);										// start conversion
	while(!(ADCSRA & (1<<ADIF)));							// waiting for ADIF, conversion complete
	return ADCW;											// Giá trị trả về từ [0 -> 1024] tương ứng [0V -> 5V]	
}

void INIT()
{
	sbi(DDR_SERVO,SERVO);
	//ADC
	ADMUX=(1<<REFS0);										// 0b0100000000 Chọn điện áp tham chiếu từ chân AVCC, thêm tụ ở AREF
	ADCSRA=(1<<ADEN) | (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	// 0b10000111 Enable ADC and set Prescaler = 128
	
		
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);			// SET OCR1A & OCR1B at BOTTOM, CLEAR at Compare Match (Non-invert), Mode 14 Fast PWM
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);				// Prescaler = 8
	ICR1 = 40000;										// Time Period = 20ms
	
	TCCR2=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS22)|(1<<CS21)|(1<<CS20);  //SET OC2 at BOTTOM, CLEAR OC2 on compare match,(non-invert), Mode 3 Fast PWM,  Prescaler = 1024
	OCR2=0;
	sei();
}