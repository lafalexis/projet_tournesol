/** @file main.cpp
 *  @brief	This is the main for the plant environment
 *			measurement station.
 *
 *  @author Alexis Laframboise (lafalexis)
 *  @bug No known bugs.
 */
/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
/*End of auto generated code by Atmel studio */

#include <Wire.h>
#include <avr/interrupt.h>
#include <string.h>

#include "rtc.h"
#include "sleep.h"
#include "status.h"
#include "memory.h"

#include "modules.h"
#include "common.h"

#define CHECKSUM_BYTES	(2)


int main();

/** @brief
 *
 *
 *  @param
 *  @return
 */
int init_setup(void);

void init_relay();

void activate_relay();

void deactivate_relay();

void init_instruments();

void activate_instruments();

void deactivate_instruments();

/** @brief
 *
 *
 *  @param
 *  @return
 */
uint16_t checksum(const uint8_t *c_ptr, size_t len);

extern volatile uint8_t wake_flag;

int main(){
	// Necessary to use int main() instead of void setup() & void loop()
	init();
	// Initializing peripherals and components
	int err = 0;

	if((err = init_setup()) != ERROR_OK){
		signal_error(err);
	}

	// Buffer to be saved on SD
	uint8_t data[TOTAL_MEAS_BYTES + CHECKSUM_BYTES] = {0};

	// Index of data in buffer
	uint8_t ix = 0;

	uint16_t crc = 0;
	data_uint64_bytes dt;

	// Program loop
	while(true){
		PRINTFUNCT;
		if (wake_flag){
			wake_flag = 0;
			err = 0;

			activate_instruments();
			delay(1000);
			
			if((err = init_modules()) != ERROR_OK){
				signal_error(err);
			}
		
			
			dt.value = DS3231_get_datetime();

			for (int i = sizeof(uint64_t) - 1; i >= 0; i--){
				data[ix++] = dt.bytes[i];
			}

			// Relay for the anemometer + delay for its activation time.
			activate_relay();
			delay(1000);

			// Reads all the modules data
			ix += exec_modules(data + ix);

			// Deactivating the relay asap because its the main power consumption element.
			deactivate_relay();
			deactivate_instruments();

			crc = checksum(data, ix);

			data[ix++] = (uint8_t)((crc & 0xFF00) >> 8);
			data[ix++] = (uint8_t)(crc & 0x00FF);

			save_frame(SAVE_FILE_NAME, data, ix);

			ix = 0;
		}

	goto_sleep();
	}
	return 0;
}

int init_setup(void){

	int err = 0;

#if SERIAL_EN
	Serial.begin(SERIAL_BAUD_RATE);
#endif

	PRINTFUNCT;

	status_blinker_init();
	init_relay();
	init_instruments();
	activate_instruments();

	Wire.begin();

	err |= rtc_init();

	err |= init_memory();

	err |= init_modules();

	/* delay to ensure proper initialisation */
	delay(500);

	status_blinker_disable();
	return err;
}


void init_relay(){
	PRINTFUNCT;
	pinMode(RELAY_9V_PIN, OUTPUT);
}

void activate_relay(){
	PRINTFUNCT;
	digitalWrite(RELAY_9V_PIN, LOW);
}

void deactivate_relay(){
	PRINTFUNCT;
	digitalWrite(RELAY_9V_PIN, HIGH);
}

void init_instruments(){
	PRINTFUNCT;
	pinMode(PT100_POWER_PIN, OUTPUT);
	pinMode(HDC1080_POWER_PIN, OUTPUT);
	pinMode(AS7262_POWER_PIN, OUTPUT);
}

void activate_instruments(){
	PRINTFUNCT;
	digitalWrite(PT100_POWER_PIN, HIGH);
	digitalWrite(HDC1080_POWER_PIN, HIGH);
	digitalWrite(AS7262_POWER_PIN, HIGH);
	delay(1000);
}

void deactivate_instruments(){
	PRINTFUNCT;
	digitalWrite(PT100_POWER_PIN, LOW);
	digitalWrite(HDC1080_POWER_PIN, LOW);
	digitalWrite(AS7262_POWER_PIN, LOW);
}


uint16_t checksum(const uint8_t *c_ptr, size_t len){
	PRINTFUNCT;
	uint16_t xsum = 0;
	while(len--){
		xsum += *(c_ptr++);
	}
	return xsum;
}