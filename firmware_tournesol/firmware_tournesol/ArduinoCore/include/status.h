/*
 * status.h
 *
 * Created: 2022-04-01 11:37:36 AM
 *  Author: Alexis Laframboise
 *
 *	This module contains functions for signaling errors
 *	with an external LED indicator via flashing codes.
 *	It also contains a function to blink continuously
 *	via an interrupt in order to inform that the system is
 *	in initialization.
 */

#ifndef STATUS_H_
#define STATUS_H_

/** @brief  Kind of a dummy not-so-dummy handler...
 *			will flash a LED with a number of flashes
 *			depending on the value of the parameter.
 *
 *  @param	error code
 */
void signal_error(int err);

/** @brief	Will make the LED indicator flash repeatedly
 *			via an interrupt.
 *
 */
void status_blinker_init(void);

/** @brief	Deactivates the LED blinking.
 *
 */
void status_blinker_disable(void);


#endif /* STATUS_H_ */