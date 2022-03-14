/*!
 * @file PT100.cpp
 *
 * This is a library for the PT100 RTD sensor 
 *
 * @author Eric Castillo-Gonzalez
 * @date   03-13-2022
 *
 */

#include "PT100.h"

PT100::PT100(float ref, int pin) {
  _control_setup.calRef = ref;
  _control_setup.pin = pin;
  _control_setup.rawVal = 0;
}

PT100::~PT100(void) {

}

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
void PT100::setCalibrationReference(float ref) { _control_setup.calRef = ref; }

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
float PT100::getCalibrationReference(void) { return _control_setup.calRef; }

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
void PT100::setPin(int pin) { _control_setup.pin = pin; }

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
int PT100::getPin(void) { return _control_setup.pin; }

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
uint8_t PT100::readTemperature(void) {
  _control_setup.rawVal = analogRead(_control_setup.pin);
  //_control_setup.tempVal =; 
  return 0;
}

