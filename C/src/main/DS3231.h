#ifndef __DS3231_H_
#define __DS3231_H_

#include <Arduino.h>

// i2c slave address of the DS3231 chip
#define DS3231_I2C_ADDR             0x68

// i2c communication timeout ms
#define DS3231_I2C_TIMEOUT_MS       100

// timekeeping registers
#define DS3231_DATETIME_SEC         0x00  /* (00-59) */
#define DS3231_DATETIME_MIN         0x01  /* (00-59) */
#define DS3231_DATETIME_HOUR        0x02  /* (00-24) */
#define DS3231_DATETIME_DAY_OF_WEEK 0x04  /* (1-7) */
#define DS3231_DATETIME_DATE        0x04  /* (01-31) */
#define DS3231_DATETIME_MONTH       0x05  /* (01-12) */
#define DS3231_DATETIME_YEAR        0x06  /* (00-99) */

#define DS3231_ALARM1_ADDR          0x07
#define DS3231_ALARM2_ADDR          0x0B
#define DS3231_CONTROL_ADDR         0x0E
#define DS3231_STATUS_ADDR          0x0F
#define DS3231_AGING_OFFSET_ADDR    0x10
#define DS3231_TEMPERATURE_MSB_ADDR 0x11
#define DS3231_TEMPERATURE_LSB_ADDR 0x12

// control register bits
#define DS3231_CONTROL_A1IE     (1 << 0)   /* Alarm 1 Interrupt Enable */
#define DS3231_CONTROL_A2IE     (1 << 1)   /* Alarm 2 Interrupt Enable */
#define DS3231_CONTROL_INTCN    (1 << 2)   /* Interrupt Control */
#define DS3231_CONTROL_RS1      (1 << 3)   /* square-wave rate select 2 */
#define DS3231_CONTROL_RS2      (1 << 4)   /* square-wave rate select 2 */
#define DS3231_CONTROL_CONV     (1 << 5)   /* Convert Temperature */
#define DS3231_CONTROL_BBSQW    (1 << 6)   /* Battery-Backed Square-Wave Enable */
#define DS3231_CONTROL_EOSC     (1 << 7)   /* not Enable Oscillator, 0 equal on */

// status register bits
#define DS3231_STATUS_A1F      (1 << 0)   /* Alarm 1 Flag */
#define DS3231_STATUS_A2F      (1 << 1)   /* Alarm 2 Flag */
#define DS3231_STATUS_BUSY     (1 << 2)   /* device is busy executing TCXO */
#define DS3231_STATUS_EN32KHZ  (1 << 3)   /* Enable 32KHz Output  */
#define DS3231_STATUS_OSF      (1 << 7)   /* Oscillator Stop Flag */

#define DS3231_CONFIG_DEFAULT { 0, 0, NULL, NULL }

typedef uint32_t DS3231_unix_time_t;

typedef struct DS3231_alm_config{
  uint8_t mask;       /* For alm 1 :
                              0xF -> once per sec,
                              0xE -> when secs match,
                              0xC -> when min and secs match,
                              0x8 -> when hours, min and secs match 
                              0x0 -> every week or month, 
                                     when h, m and s match (depends on dy_dt) 
                             For alm 2 : (Skips LSB)
                              0xF -> once per min (seconds = 00),
                              0xE -> when min match,
                              0xC -> when hour and min match,
                              0x0 -> every week or month, 
                                     when h and m match (depends on dy_dt) */
  
  uint8_t sec;          /* 0-59 (ignored for alarm 2)*/  
  uint8_t min;          /* 0-59 */
  uint8_t hour;         /* 0-23 */
  uint8_t day;          /* 1-31 */
  
} DS3231_alm_config_t;

typedef struct DS3231_config{
  uint8_t day_or_date;    /* 0 -> date of month / 1 -> day of week */
  uint8_t h12_or_h24;     /* 0 -> 24h mode      / 1 -> 12h mode */

  DS3231_alm_config_t* pAlarm_1;
  DS3231_alm_config_t* pAlarm_2;
} DS3231_config_t;

int DS3231_init(DS3231_config* pConfig);

int DS3231_set_datetime(DS3231_unix_time_t unix_time);
DS3231_unix_time_t DS3231_get_datetime(void);

#endif //__DS3231_H_
