/*!
 *@file AS7262.h
 *@brief Define the basic structure of class AS7262, the implementation of basic methods.
 *@copyright   
 *@licence     
 *@author [Alexis Laframboise](ap.laframboise@gmail.com)
 *@version  V1.0
 *@mention  Inspired from DFRobot_DHT20 library
 *@date  2022-03-02
*/

#ifndef AS7262_H
#define AS7262_H

#include <Arduino.h>
#include <Wire.h>

/*
  AS7262 I2C device information and physical registers
*/
#define AS7262_SLAVE_ADDRESS_READ 0x93
#define AS7262_SLAVE_ADDRESS_WRITE 0x92

#define AS7262_STATUS_REGISTER 0x00
#define AS7262_WRITE_REGISTER 0x01
#define AS7262_READ_REGISTER 0x02

/*
  AS7262 I2C virtual register addresses
*/
// Version registers
#define AS7262_HW_VERSION_L 0x00
#define AS7262_HW_VERSION_H 0x01
#define AS7262_FW_VERSION_L 0x02
#define AS7262_FW_VERSION_H 0x03

// Control registers
#define AS7262_CR_CONTROL_SETUP 0x04
#define AS7262_CR_INTEGRATION_TIME 0x05
#define AS7262_CR_DEVICE_TEMPERATURE 0x06
#define AS7262_CR_LED_CONTROL 0x07

// Sensor raw data register
#define AS7262_V_RAW_H 0x08
#define AS7262_V_RAW_L 0x09
#define AS7262_B_RAW_H 0x0A
#define AS7262_B_RAW_L 0x0B
#define AS7262_G_RAW_H 0x0C
#define AS7262_G_RAW_L 0x0D
#define AS7262_Y_RAW_H 0x0E
#define AS7262_Y_RAW_L 0x0F
#define AS7262_O_RAW_H 0x10
#define AS7262_O_RAW_L 0x11
#define AS7262_R_RAW_H 0x12
#define AS7262_R_RAW_L 0x13

// Sensor calibrated data register
#define AS7262_V_CAL_0 0x14
#define AS7262_V_CAL_1 0x15
#define AS7262_V_CAL_2 0x16
#define AS7262_V_CAL_3 0x17
#define AS7262_B_CAL_0 0x18
#define AS7262_B_CAL_1 0x19
#define AS7262_B_CAL_2 0x1A
#define AS7262_B_CAL_3 0x1B
#define AS7262_G_CAL_0 0x1C
#define AS7262_G_CAL_1 0x1D
#define AS7262_G_CAL_2 0x1E
#define AS7262_G_CAL_3 0x1F
#define AS7262_Y_CAL_0 0x20
#define AS7262_Y_CAL_1 0x21
#define AS7262_Y_CAL_2 0x22
#define AS7262_Y_CAL_3 0x23
#define AS7262_O_CAL_0 0x24
#define AS7262_O_CAL_1 0x25
#define AS7262_O_CAL_2 0x26
#define AS7262_O_CAL_3 0x27
#define AS7262_R_CAL_0 0x28
#define AS7262_R_CAL_1 0x29
#define AS7262_R_CAL_2 0x2A
#define AS7262_R_CAL_3 0x2B


class AS7262
{
  public:
  /*
   * @brief Construct the function
   * @param pWire IC bus pointer object and construction device, can both pass or not pass parameters, Wire in default.
   * @param address Chip IIC address, 0x38 in default.
   */
   AS7262(TwoWire *pWire = &Wire, uint8_t address = 0x49);

 /**
   * @brief init function
   * @return Return 0 if initialization succeeds, otherwise return non-zero and error code.
   */
  int begin(void);
    
 /**
   * @brief Get channel 1 calibrated value, unit: µW/cm²
   * @return 
   */
  float getCh1CalValue();
    
 /**
   * @brief Get channel 2 calibrated value, unit: µW/cm²
   * @return 
   */
  float getCh2CalValue();

 /**
   * @brief Get channel 3 calibrated value, unit: µW/cm²
   * @return 
   */
  float getCh3CalValue();

 /**
   * @brief Get channel 4 calibrated value, unit: µW/cm²
   * @return 
   */
  float getCh4CalValue();

 /**
   * @brief Get channel 5 calibrated value, unit: µW/cm²
   * @return 
   */
  float getCh5CalValue();

 /**
   * @brief Get channel 6 calibrated value, unit: µW/cm²
   * @return 
   */
  float getCh6CalValue();

 /**
   * @brief Get channel 6 calibrated value, unit: µW/cm²
   * @return 
   */
  float* getAllCalValues();
  
private:

  /**
   * @brief Write command into sensor chip 
   * @param pBuf  Data included in command
   * @param size  The number of the byte of command
   */
    void  writeCommand(const void *pBuf, size_t size);
  /**
   * @brief Write command into sensor chip 
   * @param pBuf  Data included in command
   * @param size  The number of the byte of command
   * @return      Return 0 if the reading is done, otherwise return non-zero. 
   */
    uint8_t  readData(void *pBuf, size_t size);   
    
    TwoWire *_pWire;
    uint8_t _address;
  
};

#endif
