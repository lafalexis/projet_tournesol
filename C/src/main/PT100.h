
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
  PT100(void);     // Constructor
  ~PT100(void);     // Destructor

  /*!
      @brief  
      @return 
  */
  void setCalibrationReference(float ref = PT100_CAL_REF);
  /*!
      @brief  
      @return 
  */
  float getCalibrationReference(void);
  /*!
      @brief  
      @return 
  */
  uint8_t readTemperature(void);

private:
  float calRef;
};

#endif
