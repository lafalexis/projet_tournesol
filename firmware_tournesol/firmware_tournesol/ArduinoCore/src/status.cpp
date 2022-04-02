/*
 * status.cpp
 *
 * Created: 2022-04-01 11:37:52 AM
 *  Author: aplaf
 */

#include <avr/interrupt.h>

#include "status.h"
#include "common.h"

ISR(TIMER1_COMPA_vect);

volatile int ledState = 0;
volatile int initStatus = 0;

void signal_error(int err){
	PRINTFUNCT;
	uint8_t blink_cnt = 0;
	if((err & ERROR_SD) != 0){
		blink_cnt = 1;
		} else if((err & ERROR_RTD) != 0){
		blink_cnt = 2;
		} else if((err & ERROR_ANEMOMETER) != 0){
		blink_cnt = 3;
		} else if((err & ERROR_HDC1080) != 0){
		blink_cnt = 4;
		} else if((err & ERROR_AS7262) != 0){
		blink_cnt = 5;
	}

	#if DEBUG_SIGNAL_ERROR_SERIAL
	Serial.print("Error value : ");Serial.print(err);
	Serial.print("\tBlink count : ");Serial.println(blink_cnt);
	#endif

	while(1){
		for (int i = 0; i < blink_cnt; i++){
			digitalWrite(ERROR_LED, HIGH);
			delay(ERROR_BLINK_MS);
			digitalWrite(ERROR_LED, LOW);
			delay(ERROR_BLINK_MS);
		}
		delay(ERROR_BLINK_MS * 2);
	}
}


void status_blinker_init(void){
	PRINTFUNCT;
	pinMode(ERROR_LED, OUTPUT);

	cli();
	TCCR1A = 0;                      //Reset Timer1 control Registor A

	bitClear(TCCR1B, WGM13);         //Set CTC mode
	bitSet(TCCR1B, WGM12);

	bitSet(TCCR1B, CS12);            //Set prescaler to 1024
	bitClear(TCCR1B, CS11);
	bitSet(TCCR1B, CS10);

	// Reset Timer1
	TCNT1 = 0;
	// Set compare value
	OCR1A = 976;

	bitSet(TIMSK1, OCIE1A); // Enable Timer1 compare interrupt
	sei();                  // Enable global interrupts
}

void status_blinker_disable(void){
	PRINTFUNCT;
	TIMSK1 &= ~(1 << OCIE1A); // turn off the timer interrupt
	digitalWrite(ERROR_LED, LOW);
	initStatus = 1;
}

ISR(TIMER1_COMPA_vect){
	if(!initStatus){
		if(ledState){
			ledState = 0;
			} else {
			ledState = 1;
		}
		digitalWrite(ERROR_LED, ledState);
	}
}
