/*
 * config.h
 *
 * Created: 2022-04-01 9:35:45 AM
 *  Author: Alexis laframboise
 *
 *	This file contains the configuration parameters
 *	of the system.
 */


#ifndef CONFIG_H_
#define CONFIG_H_

/* Saving definition */
#define SAVE_FILE_NAME	  ("datalog.bin")

/* Timing definitions */
#define ERROR_BLINK_MS    (200)

/* Error definitions */
#define ERROR_OK          (0)
#define ERROR_SD          (1 << 0)
#define ERROR_AS7262      (1 << 1)
#define ERROR_HDC1080     (1 << 2)
#define ERROR_RTD         (1 << 3)
#define ERROR_ANEMOMETER  (1 << 4)
#define ERROR_COUNT       (5)

/* Serial interface & debugging definitions */
#define SERIAL_BAUD_RATE		  (9600)
#define SERIAL_EN                 (1)
#define DEBUG_DS3231_SERIAL       (0 & SERIAL_EN)
#define DEBUG_AS7262_SERIAL       (1 & SERIAL_EN)
#define DEBUG_HDC1080_SERIAL      (1 & SERIAL_EN)
#define DEBUG_PT100_SERIAL        (1 & SERIAL_EN)
#define DEBUG_ANEMOMETER_SERIAL   (1 & SERIAL_EN)
#define DEBUG_SAVE_FRAME_SERIAL   (1 & SERIAL_EN)
#define DEBUG_SIGNAL_ERROR_SERIAL (1 & SERIAL_EN)

#define DEBUG_NO_SD               (1)

/* DS3231 config */
/* Alarm 1 config
	mask = 0xF;   // Every sec
	mask = 0xE;   // Sec match
	mask = 0xC;   // Min and sec match
	mask = 0x8;   // Hour, min and sec match
	mask = 0x0;   // See source file for details...
*/
#define DS3231_ALARM1_MASK        (0xE)
#define DS3231_ALARM1_SEC         (30)
#define DS3231_ALARM1_MIN         (30)
#define DS3231_ALARM1_HOUR        (12)

/* Alarm 2 config
	mask = 0xF;   // Every min
	mask = 0xE;   // Min match
	mask = 0xC;   // Hour and min match
	mask = 0x0;   // See source file for details...
*/
#define DS3231_ALARM2_MASK        (0xF)
#define DS3231_ALARM2_MIN         (30)
#define DS3231_ALARM2_HOUR        (12)

/* Timestamp related */
#define UPDATE_TIMESTAMP		  (0)
#define UNIX_CURRENT_TIMESTAMP    (1649628900)

/* Modules related */
#define AS7262_MEAS_BYTES		  (AS726x_NUM_CHANNELS * sizeof(float))
#define HDC1080_MEAS_BYTES		  (2 * sizeof(float))
#define PT100_MEAS_BYTES		  (1 * sizeof(float))
#define ANEMOMETER_MEAS_BYTES	  (1 * sizeof(float))

#define TOTAL_MEAS_BYTES		  (AS7262_MEAS_BYTES +\
								   HDC1080_MEAS_BYTES+\
								   PT100_MEAS_BYTES+\
								   ANEMOMETER_MEAS_BYTES)

#endif /* CONFIG_H_ */