/*
 * sleep.cpp
 *
 * Created: 2022-04-01 11:29:17 AM
 *  Author: aplaf
 */
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "sleep.h"
#include "common.h"

void sleepISR();

volatile uint8_t wake_flag = 0;

void goto_sleep(void){

	PRINTFUNCT;

	/*
	* Borrowed from :
	* https://github.com/RalphBacon/192-DS3231-Wake-Up-Arduino/blob/master/YouTube%20Sketch2%20Sleep/Arduino_Sleep_DS3231_Wakeup.cpp
	*/
	// Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
	static byte prevADCSRA = ADCSRA;
	ADCSRA = 0;

	/* Set the type of sleep mode we want. Can be one of (in order of power saving):
	SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
	SLEEP_MODE_ADC
	SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
	SLEEP_MODE_EXT_STANDBY
	SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
	SLEEP_MODE_PWR_DOWN (Deep sleep)
	*/
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// Turn of Brown Out Detection (low voltage)
	// Thanks to Nick Gammon for how to do this (temporarily) in software rather than
	// permanently using an avrdude command line.
	//
	// Note: Microchip state: BODS and BODSE only available for picoPower devices ATmega48PA/88PA/168PA/328P
	//
	// BODS must be set to one and BODSE must be set to zero within four clock cycles. This sets
	// the MCU Control Register (MCUCR)
	MCUCR = bit(BODS) | bit(BODSE);

	// The BODS bit is automatically cleared after three clock cycles so we better get on with it
	MCUCR = bit(BODS);

	// Ensure we can wake up again by first disabling interupts (temporarily) so
	// the wakeISR does not run before we are asleep and then prevent interrupts,
	// and then defining the ISR (Interrupt Service Routine) to run when poked awake
	noInterrupts();
	attachInterrupt(digitalPinToInterrupt(DS3231_EXTINT_PIN), sleepISR, LOW);

	// Allow interrupts now
	interrupts();

	Serial.flush();

	// And enter sleep mode as set above
	sleep_cpu();

	// Re-enable ADC if it was previously running
	ADCSRA = prevADCSRA;
}

void sleepISR(){
	wake_flag = 1;
	sleep_disable();
	detachInterrupt(digitalPinToInterrupt(DS3231_EXTINT_PIN));
}