/*
 * memory.h
 *
 * Created: 2022-04-01 11:47:53 AM
 *  Author: Alexis Laframboise
 *
 *	This module contains functions abstracting
 *	the physical layer for memory purposes.
 */

#ifndef MEMORY_H_
#define MEMORY_H_

/** @brief	Initializes the memory related peripheral.
 *
 *
 *  @return	error code
 */
int init_memory(void);

/** @brief	Saves
 *
 *
 *  @param	file name to save the data to
 *  @param	pointer to the first data byte
 *  @param	number of bytes to save
 */
void save_frame(char* fname, uint8_t* data, uint8_t len);


#endif /* MEMORY_H_ */