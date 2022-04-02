/*
 * rtc.cpp
 *
 * Created: 2022-04-01 11:56:49 AM
 *  Author: aplaf
 */
#include "rtc.h"
#include "common.h"

int ds3231_init(uint8_t set_current_time);

int rtc_init(){
	return ds3231_init(UPDATE_TIMESTAMP);
}

int ds3231_init(uint8_t set_current_time){

	int err = 0;
	PRINTFUNCT;

	DS3231_config_t ds3231_config = DS3231_CONFIG_DEFAULT;
	DS3231_alm_config_t alm1_config;
	DS3231_alm_config_t alm2_config;

	alm1_config.mask = DS3231_ALARM1_MASK;
	alm1_config.sec  = DS3231_ALARM1_SEC;
	alm1_config.min  = DS3231_ALARM1_MIN;
	alm1_config.hour = DS3231_ALARM1_HOUR;

	alm2_config.mask = DS3231_ALARM2_MASK;
	alm2_config.min  = DS3231_ALARM2_MIN;
	alm2_config.hour = DS3231_ALARM2_HOUR;

	ds3231_config.pAlarm_1 = &alm1_config;
	ds3231_config.pAlarm_2 = &alm2_config;

	#if DEBUG_DS3231_SERIAL
	Serial.print("alm1 mask : "); Serial.println(ds3231_config.pAlarm_1->mask);
	Serial.print("alm1 sec : "); Serial.println(ds3231_config.pAlarm_1->sec);
	Serial.print("alm1 min : "); Serial.println(ds3231_config.pAlarm_1->min);
	Serial.print("alm1 hours : "); Serial.println(ds3231_config.pAlarm_1->hour);
	Serial.print("alm2 mask : "); Serial.println(ds3231_config.pAlarm_2->mask);
	Serial.print("alm2 min : "); Serial.println(ds3231_config.pAlarm_2->min);
	Serial.print("alm2 hours : "); Serial.println(ds3231_config.pAlarm_2->hour);
	#endif

	err |= DS3231_init(&ds3231_config);

	pinMode(DS3231_EXTINT_PIN, INPUT_PULLUP);

	if(set_current_time){
		DS3231_set_datetime(UNIX_CURRENT_TIMESTAMP);
	}

	return err;
}