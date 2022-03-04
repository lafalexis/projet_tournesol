#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#include "Adafruit_AS726x.h"
#include "ClosedCube_HDC1080.h"

#define SAMPLING_S  (1)
#define SAMPLING_MS (1000 * (SAMPLING_S - 0.5))

#define NO_ERROR          (0)
#define SD_ERROR          (1 << 0)
#define AS7262_ERROR      (1 << 1)
#define HDC1080_ERROR     (1 << 2)
#define RTD_ERROR         (1 << 3)
#define ANEMOMETER_ERROR  (1 << 4)

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

void init_setup(void);
int as7262_init(Sensor_t* sens);
int hdc1080_init(Sensor_t* sens);
int dht20_init(Sensor_t* sens);

uint8_t as7262_read(Sensor_t* sens, uint8_t* data);
uint8_t hdc1080_read(Sensor_t* sens, uint8_t* data);
uint8_t dht20_read(Sensor_t* sens, uint8_t* data);

void save_frame(uint8_t* data, uint8_t len);
void signal_error(int err);

Sensor_t as7262;
Sensor_t hdc1080;

Adafruit_AS726x as7262_sensor;
ClosedCube_HDC1080 hdc1080_sensor;

const int chipSelect = 10;

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
  uint64_t datetime = 0;

  // Program loop
  while(true){
    data[ix++] = (datetime & 0xFF00000000000000) >> 56;
    data[ix++] = (datetime & 0x00FF000000000000) >> 48;
    data[ix++] = (datetime & 0x0000FF0000000000) >> 40;
    data[ix++] = (datetime & 0x000000FF00000000) >> 32;
    data[ix++] = (datetime & 0x00000000FF000000) >> 24;
    data[ix++] = (datetime & 0x0000000000FF0000) >> 16;
    data[ix++] = (datetime & 0x000000000000FF00) >> 8;
    data[ix++] = (datetime & 0x00000000000000FF);
    ix += as7262.sread(&as7262, data + ix);
    ix += hdc1080.sread(&hdc1080, data + ix);

    save_frame(data, ix);

    delay(SAMPLING_MS);
    datetime += SAMPLING_MS;
    ix = 0;
  }
  
  return 0;
}

int init_setup(void){
  int err = 0;
  Serial.begin(9600);
  err |= sd_init();
  err |= as7262_init(&as7262);
  err |= hdc1080_init(&hdc1080);
  return err;
}

void save_frame(uint8_t* data, uint8_t len){

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.bin", FILE_WRITE);
    Serial.print("Frame lenght : "); Serial.println(len);
    for(int i = 0; i < len; i++){
      Serial.print(data[i]); 
      Serial.print(",\t");
    }
    Serial.println();
    
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.write(data, len);
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
}

void signal_error(int err){
  int errcnt = 0;
  while(err > 0){
    errcnt += err & 1;
    err >>= 1;
  }
}

int sd_init(){
    // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return 
  }
}

int as7262_init(Sensor_t* sens){

  //Adafruit_AS726x as7262_sensor;
  if(!as7262_sensor.begin()){
    Serial.println("could not connect to sensor! Please check your wiring.");
    while(1);
  }

  sens->sensor_mod = (void*)&as7262_sensor;
  
  sens->sread = &as7262_read;
  
}

int hdc1080_init(Sensor_t* sens){
  
  hdc1080_sensor.begin(0x40);
  hdc1080_sensor.setResolution(HDC1080_RESOLUTION_11BIT, HDC1080_RESOLUTION_11BIT);

  sens->sensor_mod = (void*)&hdc1080_sensor;
  sens->sread = &hdc1080_read;
}
/*
int dht20_init(Sensor_t* sens){
  DFRobot_DHT20 dht20_sensor;
  sens->sensor_mod = (void*)&dht20_sensor;
  
  sens->sread = &dht20_read;

  dht20_sensor.begin();
}
*/
uint8_t as7262_read(Sensor_t* sens, uint8_t* data){

  float measurements[AS726x_NUM_CHANNELS] = {0};
  data_float_bytes fb;
  
  Adafruit_AS726x* pAs7262 = (Adafruit_AS726x*)sens->sensor_mod;
  
  pAs7262->startMeasurement(); //begin a measurement

  while(!pAs7262->dataReady());
  
  pAs7262->readCalibratedValues(measurements);

  for (int i = 0; i < AS726x_NUM_CHANNELS; i++){
    fb.value = measurements[i];
    /*
    Serial.print("CH: "); Serial.print(i);
    Serial.print("\t"); Serial.print(measurements[i]); Serial.print("\t");
    */
    for (int j = 0; j < sizeof(float); j++){
      data[i * sizeof(float) + j] = fb.bytes[j];
    }
  }

  //Serial.println();
  
  return 6 * sizeof(float);
}

uint8_t hdc1080_read(Sensor_t* sens, uint8_t* data){
  ClosedCube_HDC1080* pHdc1080 = (ClosedCube_HDC1080*)sens->sensor_mod;
  data_float_bytes temp;
  data_float_bytes rh;
  
  temp.value = (float)(pHdc1080->readTemperature());
  rh.value = (float)(pHdc1080->readHumidity());

  /*
  Serial.print("Temp: "); Serial.print(temp.value);
  Serial.print("\tRH: "); Serial.println(rh.value);
  */
  
  for (int i = 0; i < sizeof(float); i++){
    data[i] = temp.bytes[i];
    data[i + sizeof(float)] = rh.bytes[i];
  }
  return 2 * sizeof(float);
}
/*
uint8_t dht20_read(Sensor_t* sens, uint8_t* data){
  DFRobot_DHT20* dht20_sensor = (DFRobot_DHT20*)sens->sensor_mod;
  data_float_bytes temp;
  data_float_bytes rh;
  temp.value = dht20_sensor->getTemperature();
  rh.value = dht20_sensor->getHumidity();
  
  for(int i = 0; i < sizeof(float); i++){
    data[i] = temp.bytes[i];
    data[i + sizeof(float)] = rh.bytes[i];
    
  }
  return sizeof(float) * 2;
  
}
*/
