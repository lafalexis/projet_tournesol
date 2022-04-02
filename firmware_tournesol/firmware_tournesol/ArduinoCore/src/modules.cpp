/*
 * modules.cpp
 *
 *  Created: 2022-04-01 9:34:06 AM
 *  Authors: Alexis Laframboise
 *			 Eric Castillo-Gonzalez
 *
 *	This module contains the modules that make the
 *	measurements. It is an abstraction layer between
 *	the drivers and the application.
 */

#include <Wire.h>

#include "modules.h"


/* Convenient types definitions */
struct Sensor_t;

typedef int (*_sensor_init)(Sensor_t*);

typedef uint8_t (*_sensor_read)(Sensor_t*, uint8_t*);

typedef struct Sensor_t{
	void* sensor_mod;
	_sensor_init s_init;
	_sensor_read s_read;
} Sensor_t;

/** @brief	This function initializes the AS7262 module.
 *
 *  @param	sensor struct pointer
 *  @return	error code
 */
int _as7262_init(Sensor_t* sens);

/** @brief	This function initializes the HDC1080 module.
 *
 *  @param	sensor struct pointer
 *  @return	error code
 */
int _hdc1080_init(Sensor_t* sens);

/** @brief	This function initializes the pt100 module.
 *
 *  @param	sensor struct pointer
 *  @return	error code
 */
int _pt100_init(Sensor_t* sens);

/** @brief	This function initializes the anemometer module.
 *
 *  @param	sensor struct pointer
 *  @return	error code
 */
int _anemometer_init(Sensor_t* sens);

/** @brief	This function reads the measurements from the AS7262.
 *
 *
 *  @param	sensor struct pointer
 *  @param	byte array from main
 *  @return	number of bytes read
 */
uint8_t _as7262_read(Sensor_t* sens, uint8_t* data);

/** @brief	This function reads the measurements from the HDC1080.
 *
 *
 *  @param	sensor struct pointer
 *  @param	byte array from main
 *  @return	number of bytes read
 */
uint8_t _hdc1080_read(Sensor_t* sens, uint8_t* data);

/** @brief	This function reads the measurements from the pt100.
 *
 *
 *  @param	sensor struct pointer
 *  @param	byte array from main
 *  @return	number of bytes read
 */
uint8_t _pt100_read(Sensor_t* sens, uint8_t* data);

/** @brief	This function reads the measurements from the anemometer.
 *
 *
 *  @param	sensor struct pointer
 *  @param	byte array from main
 *  @return	number of bytes read
 */
uint8_t _anemometer_read(Sensor_t* sens, uint8_t* data);

// Modules sensor struct
Sensor_t as7262;
Sensor_t hdc1080;
Sensor_t pt100;
Sensor_t anemometer;

// Driver class instantiation
Adafruit_AS726x as7262_sensor;
ClosedCube_HDC1080 hdc1080_sensor;
PT100 pt100_sensor;
Anemometer anemometer_sensor;
/*
const Sensor_t AS7262_SENSOR = {(void*)&as7262, &_as7262_init, &_as7262_read};
const Sensor_t HDC1080_SENSOR = {(void*)&hdc1080, &_hdc1080_init, &_hdc1080_read};
const Sensor_t PT100_SENSOR = {(void*)&pt100, &_pt100_init, &_pt100_read};
const Sensor_t ANEMOMETER_SENSOR = {(void*)&anemometer, &_anemometer_init, &_anemometer_read};
*/
// Sensor struct list
Sensor_t sensor_list[] = {
	{(void*)&as7262, &_as7262_init, &_as7262_read},
	{(void*)&hdc1080, &_hdc1080_init, &_hdc1080_read},
	{(void*)&pt100, &_pt100_init, &_pt100_read},
	{(void*)&anemometer, &_anemometer_init, &_anemometer_read},
	{NULL, NULL, NULL}
};

/************************************************************************/
/*                    Sensor init functions                             */
/************************************************************************/

int _as7262_init(Sensor_t* sens){

	PRINTFUNCT;

	if(!as7262_sensor.begin()){

		#if SERIAL_EN
		Serial.print("ERROR : "); Serial.print(__FUNCTION__); Serial.println(" : Sensor unreachable.");
		#endif

		return ERROR_AS7262;
	}

	sens->sensor_mod = (void*)&as7262_sensor;

	return ERROR_OK;
}

int _hdc1080_init(Sensor_t* sens){

	PRINTFUNCT;

	hdc1080_sensor.begin(0x40);
	hdc1080_sensor.setResolution(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_11BIT);

	if(hdc1080_sensor.readDeviceId() != 0x1050){
		Serial.print("ERROR : "); Serial.print(__FUNCTION__); Serial.println(" : Sensor unreachable.");
		return ERROR_HDC1080;
	}

	sens->sensor_mod = (void*)&hdc1080_sensor;

	return ERROR_OK;
}

int _pt100_init(Sensor_t* sens){

	PRINTFUNCT;

	pt100_sensor.setPin(PT100_ADC_PIN);

	sens->sensor_mod = (void*)&pt100_sensor;

	return 0;
}

int _anemometer_init(Sensor_t* sens) {

	PRINTFUNCT;

	anemometer_sensor.setPin(ANEMO_ADC_PIN);

	sens->sensor_mod = (void*)&anemometer_sensor;

	return 0;
}

/************************************************************************/
/*                    Sensor read functions                             */
/************************************************************************/

uint8_t _as7262_read(Sensor_t* sens, uint8_t* data){

	PRINTFUNCT;

	float measurements[AS726x_NUM_CHANNELS] = {0};
	data_float_bytes fb;

	Adafruit_AS726x* pAs7262 = (Adafruit_AS726x*)sens->sensor_mod;

	pAs7262->startMeasurement(); //begin a measurement

	while(!pAs7262->dataReady());

	pAs7262->readCalibratedValues(measurements);

	for (int i = 0; i < AS726x_NUM_CHANNELS; i++){
		fb.value = measurements[i];

#if DEBUG_AS7262_SERIAL
		Serial.print("CH: "); Serial.print(i);
		Serial.print("\t"); Serial.print(measurements[i]); Serial.print("\t");
#endif

		for (int j = 0; j < sizeof(float); j++){
			data[i * sizeof(float) + j] = fb.bytes[j];
		}
	}

#if DEBUG_AS7262_SERIAL
	Serial.println();
#endif

	return AS7262_MEAS_BYTES;
}

uint8_t _hdc1080_read(Sensor_t* sens, uint8_t* data){

	PRINTFUNCT;

	ClosedCube_HDC1080* pHdc1080 = (ClosedCube_HDC1080*)sens->sensor_mod;
	data_float_bytes temp;
	data_float_bytes rh;

	temp.value = (float)(pHdc1080->readTemperature());
	rh.value = (float)(pHdc1080->readHumidity());

#if DEBUG_HDC1080_SERIAL
	Serial.print("Temp: "); Serial.print(temp.value);
	Serial.print("\tRH: "); Serial.println(rh.value);
#endif

	for (int i = 0; i < sizeof(float); i++){
		data[i] = temp.bytes[i];
		data[i + sizeof(float)] = rh.bytes[i];
	}
	return HDC1080_MEAS_BYTES;
}

uint8_t _pt100_read(Sensor_t* sens, uint8_t* data) {

	PRINTFUNCT;

	PT100* pPt100 = (PT100*)sens->sensor_mod;

	data_float_bytes temp;
	temp.value = (float)(pPt100->readTemperature());

	#if DEBUG_PT100_SERIAL
	Serial.print("Temp(PT100): "); Serial.print(temp.value); Serial.print("\n");
	#endif
	for (int i = 0; i < sizeof(float); i++){
		data[i] = temp.bytes[i];
	}
	return PT100_MEAS_BYTES;
}

uint8_t _anemometer_read(Sensor_t* sens, uint8_t* data) {

	PRINTFUNCT;

	Anemometer* pAnemometer = (Anemometer*)sens->sensor_mod;

	data_float_bytes temp;
	temp.value = (float)(pAnemometer->readWindSpeed());

#if DEBUG_ANEMOMETER_SERIAL
	Serial.print("Vit. Vent: "); Serial.print(temp.value); Serial.print("\n");
#endif

	for (int i = 0; i < sizeof(float); i++){
		data[i] = temp.bytes[i];
	}
	return ANEMOMETER_MEAS_BYTES;
}

int init_modules(void){

	PRINTFUNCT;

	int err = 0;
	int i = 0;

	while (sensor_list[i].sensor_mod != NULL){
		err |= sensor_list[i].s_init((Sensor_t*)(sensor_list[i].sensor_mod));
		i++;
	}

	return err;
}

int exec_modules(uint8_t* data){

	PRINTFUNCT;

	int i = 0;
	int ix = 0;

	while (sensor_list[i].sensor_mod != NULL){
		ix += sensor_list[i].s_read((Sensor_t*)(sensor_list[i].sensor_mod), data + ix);
		i++;
	}

	return ix;
}