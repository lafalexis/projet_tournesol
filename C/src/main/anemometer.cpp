/*!
 * @file anemometer.cpp
 *
 * This is a library for the anemometer sensor 
 *
 * @author Eric Castillo-Gonzalez
 * @date   03-27-2022
 *
 */

#include "anemometer.h"

Anemometer::Anemometer(int pin) {
	_control_setup.pin = pin;
	_control_setup.rawVal = 0;
}

Anemometer::~Anemometer() {

}

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
void Anemometer::setPin(int pin) { _control_setup.pin = pin; }

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
int Anemometer::getPin() { return _control_setup.pin; }

/**************************************************************************/
/*!
    @brief  
    @param 
    @return 
*/
/**************************************************************************/
float Anemometer::readWindSpeed() { 
  _control_setup.rawVal = analogRead(_control_setup.pin);
  // Bits to wind readWindSpeed
  _control_setup.speed = (_control_setup.rawVal * ANEMO_C1) - ANEMO_C2;
  if(_control_setup.speed < 0) {
  	_control_setup.speed = 0;
  }
  return _control_setup.speed; 
}
