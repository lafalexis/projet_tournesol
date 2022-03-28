#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "Adafruit_AS726x.h"
#include "ClosedCube_HDC1080.h"
#include "DS3231.h"
#include "anemometer.h"

#define SAMPLING_S        (5)
#define SAMPLING_MS       (1000 * (SAMPLING_S - 0.5))

#define ERROR_LED         (2)
#define DS3231_EXTINT_PIN (3)

#define NO_ERROR          (0)
#define SD_ERROR          (1 << 0)
#define AS7262_ERROR      (1 << 1)
#define HDC1080_ERROR     (1 << 2)
#define RTD_ERROR         (1 << 3)
#define ANEMOMETER_ERROR  (1 << 4)
#define ERROR_COUNT       (5)

#define ERROR_BLINK_MS    (200)

#define SERIAL_EN                 (1)
#define DEBUG_AS7262_SERIAL       (0 & SERIAL_EN)
#define DEBUG_HDC1080_SERIAL      (0 & SERIAL_EN)
#define DEBUG_SAVE_FRAME_SERIAL   (0 & SERIAL_EN)
//#define DEBUG_SIGNAL_ERROR_SERIAL (1 & SERIAL_EN)
#define DEBUG_ANEMOMETER_SERIAL   (1 & SERIAL_EN)

#define DEBUG_NO_SD               (1)

#define DS3231_ALARM1_SEC         (30)
#define DS3231_ALARM1_MIN         (30)
#define DS3231_ALARM1_HOUR        (12)
#define UNIX_CURRENT_TIMESTAMP    (1647380530)

struct Sensor_t;

typedef uint8_t (*_sensor_read)(Sensor_t*, uint8_t*);

typedef struct Sensor_t{
  void* sensor_mod;
  _sensor_read sread;
} Sensor_t;

union data_float_bytes {
  float value;
  uint8_t bytes[sizeof(float)];
};

union data_uint64_bytes {
  uint64_t value;
  uint8_t bytes[sizeof(uint64_t)];
};

int init_setup(void);

void status_blinker_init(void);
void status_blinker_disable(void);

int ds3231_init(uint8_t set_current_time);

int as7262_init(Sensor_t* sens);
int hdc1080_init(Sensor_t* sens);
int dht20_init(Sensor_t* sens);
int anemometer_init(Sensor_t* sens);

uint8_t as7262_read(Sensor_t* sens, uint8_t* data);
uint8_t hdc1080_read(Sensor_t* sens, uint8_t* data);
uint8_t dht20_read(Sensor_t* sens, uint8_t* data);
uint8_t anemometer_read(Sensor_t* sens, uint8_t* data);

void save_frame(uint8_t* data, uint8_t len);
void signal_error(int err);

void goto_sleep(void);


// TODO: check if simple checksum is sufficient, crc table takes up lots of space.
// https://stackoverflow.com/questions/22432066/how-to-use-table-based-crc-16-code
uint16_t calculate_crc16(uint16_t crc, const uint8_t *c_ptr, size_t len);
uint16_t checksum(const uint8_t *c_ptr, size_t len);

Sensor_t as7262;
Sensor_t hdc1080;
Sensor_t anemometer;

Adafruit_AS726x as7262_sensor;
ClosedCube_HDC1080 hdc1080_sensor;
Anemometer anemometer_sensor;

DS3231_config_t ds3231_config = DS3231_CONFIG_DEFAULT;
DS3231_alm_config_t alm1_config;

const int chipSelect = 10;

volatile int ledState = 0;
volatile int initStatus = 0;
volatile uint8_t wake_flag = 0;

int main(){
  // Necessary to use int main() instead of void setup() & void loop()
  init();

  // Initializing peripherals and components
  int err = 0;
  if((err = init_setup()) != NO_ERROR){
    signal_error(err);
  }

  // Buffer to be saved on SD
  uint8_t data[64] = {0};
  uint8_t ix = 0;
  uint16_t crc = 0;
  //debug
  uint64_t datetime = 0;
  data_uint64_bytes dt;

  // Program loop
  while(true){

    //dt.value = DS3231_get_datetime();
    dt.value = datetime;
    
    for (int i = sizeof(uint64_t) - 1; i >= 0; i--){
      data[ix++] = dt.bytes[i];
    }

    //ix += as7262.sread(&as7262, data + ix);
    //ix += hdc1080.sread(&hdc1080, data + ix);
    ix += anemometer.sread(&anemometer, data + ix);
    
    //crc = calculate_crc16(0xFFFF, data, ix);
    crc = checksum(data, ix);
    data[ix++] = (uint8_t)((crc & 0xFF00) >> 8);
    data[ix++] = (uint8_t)(crc & 0x00FF);

    save_frame(data, ix);
    //debug
    delay(SAMPLING_MS);
    datetime += SAMPLING_MS;
    ix = 0;
    //Serial.println("going to sleep");
    //goto_sleep();
  }
  
  return 0;
}

int init_setup(void){
  int err = 0;
  status_blinker_init();

  // debug
  //ds3231_init(0);
  
#if SERIAL_EN
  Serial.begin(9600);
  Serial.print("test debut\n");
#endif

#if !DEBUG_NO_SD
  err |= sd_init();
#endif

  //err |= as7262_init(&as7262);
  //err |= hdc1080_init(&hdc1080);
  err |= anemometer_init(&anemometer);
  
  delay(500);
  status_blinker_disable();
  return err;
}

uint16_t checksum(const uint8_t *c_ptr, size_t len){
  uint16_t xsum = 0;
  while(len--){
    xsum += *(c_ptr++);
  }
  return xsum;
}

void save_frame(uint8_t* data, uint8_t len){

#if !DEBUG_NO_SD
    File dataFile = SD.open("datalog.bin", FILE_WRITE);
    dataFile.write(data, len);
    dataFile.close();
#endif

#if DEBUG_SAVE_FRAME_SERIAL
    Serial.print("Frame lenght : "); Serial.println(len);
    for(int i = 0; i < len; i++){
      Serial.print(data[i]); 
      Serial.print(",\t");
    }
    Serial.println();
#endif
}

void signal_error(int err){
  uint8_t blink_cnt = 0;
  if((err & SD_ERROR) != 0){
    blink_cnt = 1;
  } else if((err & RTD_ERROR) != 0){
    blink_cnt = 2;
  } else if((err & ANEMOMETER_ERROR) != 0){
    blink_cnt = 3;
  } else if((err & HDC1080_ERROR) != 0){
    blink_cnt = 4;
  } else if((err & AS7262_ERROR) != 0){
    blink_cnt = 5;
  }
  
#if DEBUG_SIGNAL_ERROR_SERIAL
  Serial.print("Error value : ");Serial.print(err);
  Serial.print("\tBlink count : ");Serial.println(blink_cnt);
#endif

  while(1){
    for (int i = 0; i < blink_cnt; i++){
      digitalWrite(ERROR_LED, HIGH);
      delay(ERROR_BLINK_MS);
      digitalWrite(ERROR_LED, LOW);
      delay(ERROR_BLINK_MS);
    }
    delay(ERROR_BLINK_MS * 2);
  }
}

int ds3231_init(uint8_t set_current_time){
  //alm1_config.mask = 0xF;   // Every sec
  alm1_config.mask = 0xE;   // Sec match
  //alm1_config.mask = 0xC;   // Min and sec match
  //alm1_config.mask = 0x8;   // Hour, min and sec match
  //alm1_config.mask = 0x0;   // See source file for details...

  alm1_config.sec = DS3231_ALARM1_SEC;
  alm1_config.min = DS3231_ALARM1_MIN;
  alm1_config.hour = DS3231_ALARM1_HOUR;
  ds3231_config.pAlarm_1 = &alm1_config;
  ds3231_config.pAlarm_2 = NULL;
  
  Serial.print("alm1 mask : "); Serial.println(ds3231_config.pAlarm_1->mask); 
  Serial.print("alm1 sec : "); Serial.println(ds3231_config.pAlarm_1->sec); 
  Serial.print("alm1 min : "); Serial.println(ds3231_config.pAlarm_1->min); 
  Serial.print("alm1 hours : "); Serial.println(ds3231_config.pAlarm_1->hour); 
  DS3231_init(&ds3231_config);

  pinMode(DS3231_EXTINT_PIN, INPUT_PULLUP);

  if (set_current_time){
    DS3231_set_datetime(UNIX_CURRENT_TIMESTAMP); 
  }
}

void status_blinker_init(void){

  pinMode(ERROR_LED, OUTPUT);
  
  cli();
  TCCR1A = 0;                      //Reset Timer1 control Registor A

  bitClear(TCCR1B, WGM13);         //Set CTC mode
  bitSet(TCCR1B, WGM12);

  bitSet(TCCR1B, CS12);            //Set prescaler to 1024
  bitClear(TCCR1B, CS11);
  bitSet(TCCR1B, CS10);

  // Reset Timer1
  TCNT1 = 0;
  // Set compare value
  OCR1A = 976;

  bitSet(TIMSK1, OCIE1A); // Enable Timer1 compare interrupt
  sei();                  // Enable global interrupts
}

void status_blinker_disable(void){
  TIMSK1 &= ~(1 << OCIE1A); // turn off the timer interrupt
  digitalWrite(ERROR_LED, LOW);
  initStatus = 1;
}

int sd_init(){
    // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    
#if SERIAL_EN
    Serial.print("ERROR : "); Serial.print(__FUNCTION__); Serial.println(" : SD card unreachable.");
#endif

    return SD_ERROR;
  }
  return NO_ERROR;
}

int as7262_init(Sensor_t* sens){

  if(!as7262_sensor.begin()){
    
#if SERIAL_EN
    Serial.print("ERROR : "); Serial.print(__FUNCTION__); Serial.println(" : Sensor unreachable.");
#endif

    return AS7262_ERROR;
  }

  sens->sensor_mod = (void*)&as7262_sensor;
  
  sens->sread = &as7262_read;

  return NO_ERROR;
}

int hdc1080_init(Sensor_t* sens){
  
  hdc1080_sensor.begin(0x40);
  hdc1080_sensor.setResolution(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_11BIT);

  if(hdc1080_sensor.readDeviceId() != 0x1050){
    Serial.print("ERROR : "); Serial.print(__FUNCTION__); Serial.println(" : Sensor unreachable.");
    return HDC1080_ERROR;
  }
  
  sens->sensor_mod = (void*)&hdc1080_sensor;
  sens->sread = &hdc1080_read;

  return NO_ERROR;
}

int anemometer_init(Sensor_t* sens) {

  sens->sensor_mod = (void*)&anemometer_sensor;
  sens->sread = &anemometer_read;

  return 0;
}

uint8_t as7262_read(Sensor_t* sens, uint8_t* data){

  float measurements[AS726x_NUM_CHANNELS] = {0};
  data_float_bytes fb;
  
  Adafruit_AS726x* pAs7262 = (Adafruit_AS726x*)sens->sensor_mod;
  
  pAs7262->startMeasurement(); //begin a measurement

  while(!pAs7262->dataReady());
  
  pAs7262->readCalibratedValues(measurements);

  for (int i = 0; i < AS726x_NUM_CHANNELS; i++){
    fb.value = measurements[i];
    
#if DEBUG_AS7262_SERIAL
    Serial.print("CH: "); Serial.print(i);
    Serial.print("\t"); Serial.print(measurements[i]); Serial.print("\t");
#endif

    for (int j = 0; j < sizeof(float); j++){
      data[i * sizeof(float) + j] = fb.bytes[j];
    }
  }
  
#if DEBUG_AS7262_SERIAL
  Serial.println();
#endif

  return AS726x_NUM_CHANNELS * sizeof(float);
}

uint8_t hdc1080_read(Sensor_t* sens, uint8_t* data){
  ClosedCube_HDC1080* pHdc1080 = (ClosedCube_HDC1080*)sens->sensor_mod;
  data_float_bytes temp;
  data_float_bytes rh;
  
  temp.value = (float)(pHdc1080->readTemperature());
  rh.value = (float)(pHdc1080->readHumidity());

#if DEBUG_HDC1080_SERIAL
  Serial.print("Temp: "); Serial.print(temp.value);
  Serial.print("\tRH: "); Serial.println(rh.value);
#endif
  
  for (int i = 0; i < sizeof(float); i++){
    data[i] = temp.bytes[i];
    data[i + sizeof(float)] = rh.bytes[i];
  }
  return 2 * sizeof(float);
}

uint8_t anemometer_read(Sensor_t* sens, uint8_t* data) {
  Anemometer* pAnemometer = (Anemometer*)sens->sensor_mod;

  data_float_bytes temp;
  temp.value = (float)(pAnemometer->readWindSpeed());

#if DEBUG_ANEMOMETER_SERIAL
  Serial.print("Vit. Vent: "); Serial.print(temp.value); Serial.print("\n");
#endif
  for (int i = 0; i < sizeof(float); i++){
    data[i] = temp.bytes[i];
  }
  return sizeof(float);
}

/*
uint16_t calculate_crc16(uint16_t crc, const uint8_t *c_ptr, size_t len){
  const uint8_t *c = c_ptr;

  while (len--){
      crc = (crc << 8) ^ crctable[((crc >> 8) ^ *c++)];
  }
  return crc;
}
*/

void goto_sleep(void){
    /*
     * Borrowed from : 
     * https://github.com/RalphBacon/192-DS3231-Wake-Up-Arduino/blob/master/YouTube%20Sketch2%20Sleep/Arduino_Sleep_DS3231_Wakeup.cpp
     */
    // Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
    static byte prevADCSRA = ADCSRA;
    ADCSRA = 0;

    /* Set the type of sleep mode we want. Can be one of (in order of power saving):
     SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
     SLEEP_MODE_ADC
     SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
     SLEEP_MODE_EXT_STANDBY
     SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
     SLEEP_MODE_PWR_DOWN (Deep sleep)
     */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Turn of Brown Out Detection (low voltage)
    // Thanks to Nick Gammon for how to do this (temporarily) in software rather than
    // permanently using an avrdude command line.
    //
    // Note: Microchip state: BODS and BODSE only available for picoPower devices ATmega48PA/88PA/168PA/328P
    //
    // BODS must be set to one and BODSE must be set to zero within four clock cycles. This sets
    // the MCU Control Register (MCUCR)
    MCUCR = bit(BODS) | bit(BODSE);

    // The BODS bit is automatically cleared after three clock cycles so we better get on with it
    MCUCR = bit(BODS);

    // Ensure we can wake up again by first disabling interupts (temporarily) so
    // the wakeISR does not run before we are asleep and then prevent interrupts,
    // and then defining the ISR (Interrupt Service Routine) to run when poked awake
    noInterrupts();
    attachInterrupt(digitalPinToInterrupt(3), sleepISR, LOW);

    // Allow interrupts now
    interrupts();

    Serial.flush();

    // And enter sleep mode as set above
    sleep_cpu();

    // Re-enable ADC if it was previously running
    ADCSRA = prevADCSRA;
}

ISR(TIMER1_COMPA_vect){
  if(!initStatus){
    if(ledState){
      ledState = 0;
    } else {
      ledState = 1;
    }
    digitalWrite(ERROR_LED, ledState);
  }
}

void sleepISR(){
  wake_flag = 1;
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(DS3231_EXTINT_PIN));
  Serial.println("Waking up");
}
