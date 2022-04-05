/*
 * common.h
 *
 * Created: 2022-04-01 9:52:37 AM
 *  Author: aplaf
 */
#ifndef COMMON_H_
#define COMMON_H_

#include <Arduino.h>

#include "config.h"
#include "connections.h"

#define PRINTFUNCT        Serial.println(__FUNCTION__)

union data_float_bytes {
	float value;
	uint8_t bytes[sizeof(float)];
};

union data_uint64_bytes {
	uint64_t value;
	uint8_t bytes[sizeof(uint64_t)];
};


#endif /* COMMON_H_ */