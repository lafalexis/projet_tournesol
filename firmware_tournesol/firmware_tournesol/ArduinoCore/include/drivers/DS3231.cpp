/*!
 * @file DS3231.h
 *
 * This is a library for the DS3231 RTC.
 * It allows to use only unixtime in order
 * to set and get current time.
 *
 * @author Alexis Laframboise
 * @date   03-20-2022
 *
 */

#include "DS3231.h"
#include <Wire.h>
#include <time.h>

#define TIME_CALC_START_YEAR  (2000)
#define YEAR_2000_IN_SECONDS  (946702800)

const uint8_t DAYS_IN_MONTH[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const uint8_t DS3231_ALARM_ADDR[] = { DS3231_ALARM1_ADDR, DS3231_ALARM2_ADDR };

struct timestamp {
uint8_t sec;                    /* seconds */
uint8_t min;                    /* minutes */
uint8_t hour;                   /* hours */
uint8_t day;                    /* day of the month */
uint8_t mon;                    /* month */
int16_t year;                   /* year */
DS3231_unix_time_t unixtime;    /* seconds since 01.01.1970 00:00:00 UTC*/
};

struct timestamp ts = { 0 };

/** @brief  This function is only useful for
 *			first initialization. It sets every
 *			register to 0.
 *
 */
void _reset_all_reg(void);

/** @brief	Sets the value of a register.
 *
 *  @param	address of the register.
 *  @param	value to set the register to.
 */
void _set_reg(const uint8_t addr, const uint8_t val);

/** @brief	Gets the value of a register.
 *
 *
 *  @param	address of the register
 *  @return	value of the register
 */
uint8_t _get_reg(const uint8_t addr);

/** @brief	This function sets the alarm mask for either
 *			DS3231 alarm configuration.
 *
 *  @param	mask : The value of the mask
 *  @param	alarm_num : either 0 for alm1 or 1 for alm2.
 *  @return	0 if no error, -1 if error
 */
int _set_alarm_mask(uint8_t mask, uint8_t alarm_num);

/** @brief  This function works with the global variable
 *			ts. It takes the unixtime field and fills the
 *			datetime fields.
 *
 */
void _unix_to_datetime(void);

/** @brief  This function works with the global variable
 *			ts. It takes the datetime fields to calculate
 *			the unixtime field.
 *
 */
void _datetime_to_unix(void);

/** @brief	This function converts decimal value
 *			to bcd.
 *
 *  @param	decimal value
 *  @return bcd value
 */
uint8_t _dec2bcd(uint8_t dec);

/** @brief  This function gets a 2 digit bcd value
 *			and returns the integer value.
 *
 *  @param	bcd value
 *  @return decimal value
 */
uint8_t _bcd2dec(uint8_t bcd);

int DS3231_init(DS3231_config* pConfig) {
	//_reset_all_reg();

	int err = 0;
	// Control registers initialization.
	uint8_t control_reg = 0;

	if (pConfig->pAlarm_1 != NULL) {
		control_reg |= DS3231_CONTROL_A1IE;
		_set_reg(DS3231_ALARM1_ADDR, _dec2bcd(pConfig->pAlarm_1->sec));
		_set_reg(DS3231_ALARM1_ADDR + 1, _dec2bcd(pConfig->pAlarm_1->min));
		_set_reg(DS3231_ALARM1_ADDR + 2, _dec2bcd(pConfig->pAlarm_1->hour));
		_set_reg(DS3231_ALARM1_ADDR + 3, _dec2bcd(pConfig->pAlarm_1->day));
		err |= _set_alarm_mask(pConfig->pAlarm_1->mask, 0);
	}
	if (pConfig->pAlarm_2 != NULL) {
		control_reg |= DS3231_CONTROL_A2IE;
		_set_reg(DS3231_ALARM2_ADDR, _dec2bcd(pConfig->pAlarm_2->min));
		_set_reg(DS3231_ALARM2_ADDR + 1, _dec2bcd(pConfig->pAlarm_2->hour));
		_set_reg(DS3231_ALARM2_ADDR + 2, _dec2bcd(pConfig->pAlarm_2->day));
		err |= _set_alarm_mask(pConfig->pAlarm_2->mask, 1);
	}
	if (control_reg) {
		control_reg |= DS3231_CONTROL_INTCN;
	}

	_set_reg(DS3231_CONTROL_ADDR, control_reg);
	return err;
}

void DS3231_set_datetime(DS3231_unix_time_t unixtime) {
	ts.unixtime = unixtime;
	_unix_to_datetime();
	_set_reg(DS3231_DATETIME_SEC, _dec2bcd(ts.sec));
	_set_reg(DS3231_DATETIME_MIN, _dec2bcd(ts.min));
	_set_reg(DS3231_DATETIME_HOUR, _dec2bcd(ts.hour));
	_set_reg(DS3231_DATETIME_DATE, _dec2bcd(ts.day));
	_set_reg(DS3231_DATETIME_MONTH, _dec2bcd(ts.mon));
	_set_reg(DS3231_DATETIME_YEAR, _dec2bcd(ts.year - TIME_CALC_START_YEAR));
}

DS3231_unix_time_t DS3231_get_datetime(void) {

	uint8_t sts_reg = _get_reg(DS3231_STATUS_ADDR);

	if ((sts_reg & DS3231_STATUS_A1F) != 0){
		_set_reg(DS3231_STATUS_ADDR | DS3231_STATUS_A1F, 0);
	}

	if ((sts_reg & DS3231_STATUS_A2F) != 0){
		_set_reg(DS3231_STATUS_ADDR | DS3231_STATUS_A2F, 0);
	}


	ts.sec = _bcd2dec(_get_reg(DS3231_DATETIME_SEC));
	ts.min = _bcd2dec(_get_reg(DS3231_DATETIME_MIN));
	ts.hour = _bcd2dec(_get_reg(DS3231_DATETIME_HOUR));
	ts.day = _bcd2dec(_get_reg(DS3231_DATETIME_DATE));
	ts.mon = _bcd2dec(_get_reg(DS3231_DATETIME_MONTH));
	ts.year = _bcd2dec(_get_reg(DS3231_DATETIME_YEAR)) + TIME_CALC_START_YEAR;

	_datetime_to_unix();

	return ts.unixtime;
}

void _reset_all_reg(void) {
	for (int i = 0; i < 0x12; i++) {
	_set_reg(i, 0);
	}
}

void _set_reg(const uint8_t addr, const uint8_t val) {
	Wire.beginTransmission(DS3231_I2C_ADDR);
	Wire.write(addr);
	Wire.write(val);
	Wire.endTransmission();
}

uint8_t _get_reg(const uint8_t addr) {
	uint8_t retval = 0;

	Wire.beginTransmission(DS3231_I2C_ADDR);
	Wire.write(addr);
	Wire.endTransmission();

	uint8_t got_data = false;
	uint32_t start = millis(); // start timeout
	while (millis() - start < DS3231_I2C_TIMEOUT_MS) {
		if (Wire.requestFrom(DS3231_I2C_ADDR, 1) == 1) {
			got_data = true;
			break;
		}
	}
	if (!got_data)
		return 0; // error timeout

	retval = Wire.read();

	return retval;
}

int _set_alarm_mask(uint8_t mask, uint8_t alarm_num) {

	if (alarm_num >= DS3231_ALARM_COUNT){
		return -1;
	}

	uint8_t alm_base_addr = DS3231_ALARM_ADDR[alarm_num];
	uint8_t reg = 0;

	for (int i = alarm_num; i < 4; i++){
		reg = _get_reg(alm_base_addr);
		reg |= ((mask >> i) & 1) << 7;
		_set_reg(alm_base_addr++, reg);
	}

	return 0;
}

void _unix_to_datetime(void) {

	DS3231_unix_time_t seconds = ts.unixtime - YEAR_2000_IN_SECONDS;
	uint16_t year = TIME_CALC_START_YEAR;
	int i = 0;

	while (seconds > (SECONDS_IN_DAY * DAYS_IN_YEAR)) {
	if (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0))) {
		//Serial.print(year); Serial.println("Leap year!");
		seconds -= SECONDS_IN_DAY * DAYS_IN_LEAP_YEAR;
		year++;
	}
	else {
		//Serial.print(year); Serial.println("Normal year...");
		seconds -= SECONDS_IN_DAY * DAYS_IN_YEAR;
		year++;
	}
	}

	ts.year = year;

	while (seconds > (DAYS_IN_MONTH[i] * SECONDS_IN_DAY)) {
	seconds -= DAYS_IN_MONTH[i++] * SECONDS_IN_DAY;
	}

	ts.mon = i + 1;
	ts.day = seconds / SECONDS_IN_DAY + 1;

	seconds %= SECONDS_IN_DAY;
	ts.sec = seconds % SECONDS_IN_MINUTE;
	ts.min = seconds % SECONDS_IN_HOUR / SECONDS_IN_MINUTE;
	ts.hour = seconds / SECONDS_IN_HOUR;
}

void _datetime_to_unix(void) {
	uint64_t unixtime = ts.sec + (ts.min * SECONDS_IN_MINUTE) + (ts.hour * SECONDS_IN_HOUR);
	uint8_t leap_year = (((ts.year % 4) == 0) && (((ts.year % 100) != 0) || ((ts.year % 400) == 0)));

	// Calculate time for complete years since 2000.
	for (int i = TIME_CALC_START_YEAR; i < ts.year; i++) {
	if (((i % 4) == 0) && (((i % 100) != 0) || ((i % 400) == 0))) {
		unixtime += DAYS_IN_LEAP_YEAR * SECONDS_IN_DAY;
	}
	else {
		unixtime += DAYS_IN_YEAR * SECONDS_IN_DAY;
	}
	}

	// Calculate time for complete months for current year.
	for (int i = 0; i < ts.mon - 1; i++) {
	unixtime += DAYS_IN_MONTH[i] * SECONDS_IN_DAY;
	}

	// Calculate time for complete days in current month
	unixtime += (ts.day - 1) * SECONDS_IN_DAY;

	// If current month is march or later in year and year is a leap year, add a day.
	if (leap_year && ts.mon > 2) {
	unixtime += SECONDS_IN_DAY;
	}

	ts.unixtime = unixtime + YEAR_2000_IN_SECONDS;
}

uint8_t _dec2bcd(uint8_t dec) {
	uint8_t bcd = 0;

	bcd = (dec % 10);
	dec /= 10;
	bcd |= (dec % 10) << 4;

	return bcd;
}

uint8_t _bcd2dec(uint8_t bcd) {
	uint8_t dec = 0;

	dec = ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);

	return dec;
}
