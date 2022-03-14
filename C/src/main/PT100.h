/*!
 * @file PT100.h
 *
 * This is a library for the PT100 RTD sensor 
 *
 * @author Eric Castillo-Gonzalez
 * @date   03-13-2022
 *
 */

#ifndef LIB_PT100
#define LIB_PT100

#include "Arduino.h"

#define PT100_CAL_REF            (float) (2.77)
#define PT100_DEFAULT_PIN        (A0)           // pins_arduino.h

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with PT100
    sensor
*/
/**************************************************************************/
class PT100 {
public:           // Access specifier
  /*!
      @brief  Class constructor
      @param  None 
     
  */
  PT100(float ref = PT100_CAL_REF, int pin = PT100_DEFAULT_PIN);     // Constructor
  ~PT100(void);     // Destructor

  /*!
      @brief  
      @return 
  */
  void setCalibrationReference(float ref);
  /*!
      @brief  
      @return 
  */
  float getCalibrationReference(void);
  /*!
      @brief  
      @return 
  */
  void setPin(int pin);
  /*!
      @brief  
      @return 
  */
  int getPin(void);
  /*!
      @brief  
      @return 
  */
  uint8_t readTemperature(void);

private:
  struct control_setup {
    float calRef;
    int pin;
    int rawVal;
    float tempVal;
  } _control_setup;
};

#endif
