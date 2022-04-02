/*
 * modules.h
 *
 * Created: 2022-04-01 9:34:06 AM
 *  Author: Alexis Laframboise
 *
 *	This module contains the modules that make the
 *	measurements. It is an abstraction layer between
 *	the drivers and the application.
 */

#ifndef MODULES_H_
#define MODULES_H_

/** @brief
 *
 *
 *  @return
 */
int init_modules(void);

/** @brief
 *
 *
 *  @param
 *  @return
 */
int exec_modules(uint8_t* data);

#endif /* MODULES_H_ */