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


int main();

/** @brief
 *
 *
 *  @param
 *  @return
 */
int init_setup(void);

/** @brief
 *
 *
 *  @param
 *  @return
 */
int ds3231_init(uint8_t set_current_time);

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
	uint8_t data[64] = {0};
	// Index of data in buffer
	uint8_t ix = 0;

	uint16_t crc = 0;
	data_uint64_bytes dt;

	// Program loop
	while(true){

		PRINTFUNCT;
		if (wake_flag){
			digitalWrite(ERROR_LED, HIGH);
			wake_flag = 0;
			dt.value = DS3231_get_datetime();

			for (int i = sizeof(uint64_t) - 1; i >= 0; i--){
				data[ix++] = dt.bytes[i];
			}

			// Reads all the modules data
			ix += exec_modules(data + ix);

			crc = checksum(data, ix);

			data[ix++] = (uint8_t)((crc & 0xFF00) >> 8);
			data[ix++] = (uint8_t)(crc & 0x00FF);

			save_frame("datalog.bin", data, ix);
			ix = 0;
		}
	digitalWrite(ERROR_LED, LOW);
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

	Wire.begin();

	err |= rtc_init();

	err |= init_memory();

	err |= init_modules();

	/* delay to ensure proper initialisation */
	delay(500);

	status_blinker_disable();
	return err;
}

uint16_t checksum(const uint8_t *c_ptr, size_t len){
	PRINTFUNCT;
	uint16_t xsum = 0;
	while(len--){
		xsum += *(c_ptr++);
	}
	return xsum;
}