/*
 * modules.h
 *
 *  Created: 2022-04-01 9:34:06 AM
 *  Authors: Alexis Laframboise
 *			 Eric Castillo-Gonzalez
 *
 *	This module contains the modules that make the
 *	measurements. It is an abstraction layer between
 *	the drivers and the application.
 */

#ifndef MODULES_H_
#define MODULES_H_

#include "common.h"

#include "drivers/Adafruit_AS726x.h"
#include "drivers/ClosedCube_HDC1080.h"
#include "drivers/DS3231.h"
#include "drivers/PT100.h"
#include "drivers/anemometer.h"

/** @brief	Runs the init function for every
 *			module
 *
 *  @return error code
 */
int init_modules(void);

/** @brief	Runs the read function for every
 *			module
 *
 *  @param	a pointer to an array of bytes.
 *  @return	error code
 */
int exec_modules(uint8_t* data);

#endif /* MODULES_H_ */