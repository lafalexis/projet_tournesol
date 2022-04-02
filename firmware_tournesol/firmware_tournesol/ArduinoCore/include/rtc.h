/*
 * rtc.h
 *
 * Created: 2022-04-01 11:56:49 AM
 *  Author: Alexis Laframboise
 *
 *	This module is an abstraction layer in order
 *	to help dissociate the application from the system.
 *	This way, if we were to change the DS3231, we wouldn't
 *	even have to modify the main.
 *
 */

#ifndef RTC_H_
#define RTC_H_

#include "drivers/DS3231.h"

/** @brief	Initializes the RTC.
 *
 *  @return	error code
 */
int rtc_init(void);

#endif /* RTC_H_ */