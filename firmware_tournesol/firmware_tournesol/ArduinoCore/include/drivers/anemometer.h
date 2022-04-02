/*!
 * @file anemometer.h
 *
 * This is a library for the anemometer sensor
 *
 * @author Eric Castillo-Gonzalez
 * @date   03-27-2022
 *
 */

#ifndef LIB_ANEMO
#define LIB_ANEMO

#include <Arduino.h>

#define ANEMOMETER_DEFAULT_PIN      (A0)

// Conversion constants
#define ANEMO_C1                 (float) (0.098974)
#define ANEMO_C2                 (float) (8.1)

/**************************************************************************/
/*!
    @brief  Class that stores state and functions for interacting with the
    anemometer
*/
/**************************************************************************/
class Anemometer {
public:              // Access specifier
  /*!
    @brief  Class constructor
    @param  None
  */
  Anemometer(int pin = ANEMOMETER_DEFAULT_PIN); //  Constructor
  ~Anemometer(); // Destructor

  /*!
    @brief
    @return
  */
  void setPin(int pin);

  /*!
    @brief
    @return
  */
  int getPin();

  /*!
    @brief
    @return
  */
  float readWindSpeed();

private:
  struct control_setup {
    float calRef;
    int pin;
    int rawVal;
    float speed;
  } _control_setup;
};

#endif
