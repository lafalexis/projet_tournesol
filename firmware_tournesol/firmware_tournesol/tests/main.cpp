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

#include "status.h"
#include "memory.h"

#include "common.h"

#include "drivers/Adafruit_AS726x.h"
#include "drivers/ClosedCube_HDC1080.h"
#include "drivers/PT100.h"
#include "drivers/anemometer.h"

// Driver class instantiation
Adafruit_AS726x as7262_sensor;
ClosedCube_HDC1080 hdc1080_sensor;
PT100 pt100_sensor;
Anemometer anemometer_sensor;


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

int test_as7262();

int main(){
	// Necessary to use int main() instead of void setup() & void loop()
	init();
	// Initializing peripherals and components
	int err = 0;

	if((err = init_setup()) != ERROR_OK){
		signal_error(err);
	}

	
	test_as7262();
	
	
	return 0;
}

int test_as7262(){
	int err = 0;
	int i = 0;
	float as7262_data[AS726x_NUM_CHANNELS] = { 0 };
	
	if(!as7262_sensor.begin()){

		#if SERIAL_EN
		Serial.print("ERROR : "); Serial.print(__FUNCTION__); Serial.println(" : Sensor unreachable.");
		#endif

		return ERROR_AS7262;
	}
	
	as7262_sensor.startMeasurement(); //begin a measurement

	while(!as7262_sensor.dataReady());
	
	as7262_sensor.readCalibratedValues(as7262_data);
	
		
	while(1){
		Serial.print("CH "); Serial.print(i); Serial.print(" : "); Serial.print(as7262_data[i]); Serial.println(",");
		i++;
		if (i > 6)
		{
			break;
		}
	}
	
	return err;
}

int init_setup(void){

	int err = 0;

	Serial.begin(SERIAL_BAUD_RATE);

	PRINTFUNCT;

	init_relay();
	init_instruments();
	activate_instruments();

	Wire.begin();

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