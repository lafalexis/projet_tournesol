/*
 * memory.cpp
 *
 * Created: 2022-04-01 11:48:03 AM
 *  Author: Alexis Laframboise
 */

#include "common.h"

#include <SPI.h>
#include <SD.h>

int sd_init();

int init_memory(){
	PRINTFUNCT;
	return sd_init();
}

void save_frame(char* fname, uint8_t* data, uint8_t len){
	PRINTFUNCT;
#if !DEBUG_NO_SD
	File dataFile = SD.open(fname, FILE_WRITE);
	dataFile.write(data, len);
	dataFile.close();
#endif

#if DEBUG_SAVE_FRAME_SERIAL
	Serial.print("File name : "); Serial.println(fname);
	Serial.print("Frame lenght : "); Serial.println(len);
	for(int i = 0; i < len; i++){
		Serial.print(data[i]);
		Serial.print(",\t");
	}
	Serial.println();
#endif
}

int sd_init(){
	PRINTFUNCT;

#if !DEBUG_NO_SD
	// see if the card is present and can be initialized:
	if (!SD.begin(SD_CHIP_SELECT_PIN)) {

		#if SERIAL_EN
		Serial.print("ERROR : "); Serial.print(__FUNCTION__); Serial.println(" : SD card unreachable.");
		#endif

		return ERROR_SD;
	}
#endif
	return ERROR_OK;
}