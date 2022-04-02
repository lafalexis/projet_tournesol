/*
 * sleep.h
 *
 * Created: 2022-04-01 9:34:06 AM
 *  Author: Alexis Laframboise
 *
 *	This module allows for the arduino to go into power down
 *	sleep mode and be awaken by an interrupt raised by the external RTC.
 *
 */

#ifndef SLEEP_H_
#define SLEEP_H_

/** @brief	Sends the microcontroller in sleep mode.
 *			attaches a software interrupt to the RTC
 *			interrupt pin.
 *	@source	https://github.com/RalphBacon/192-DS3231-Wake-Up-Arduino/blob/master/YouTube%20Sketch2%20Sleep/Arduino_Sleep_DS3231_Wakeup.cpp
 */
void goto_sleep(void);

#endif /* SLEEP_H_ */