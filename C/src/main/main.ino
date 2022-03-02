#include <DFRobot_DHT20.h>
#include <SPI.h>
#include <SD.h>

#define SAMPLING_S  1
#define SAMPLING_MS 1000 * SAMPLING_S

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

bool as7262_init(Sensor_t* sens);
uint8_t as7262_read(Sensor_t* sens, uint8_t* data);

bool dht20_init(Sensor_t* sens);
uint8_t dht20_read(Sensor_t* sens, uint8_t* data);

void init_setup(void);

void save_frame(uint8_t* data, uint8_t len);

Sensor_t as7262;
Sensor_t dht20;

const int chipSelect = 10;

int main(){
  init();
  init_setup();

  uint8_t data[32] = {0};
  uint8_t ix = 0;

  while(true){
    Serial.println("Printing indexes: ");
    ix += as7262.sread(&as7262, data + ix);
    Serial.println(ix);
    ix += dht20.sread(&dht20, data + ix);
    Serial.println(ix);
    Serial.println("Printing data[32]: ");
    for(int i = 0; i < 32; i++){
      Serial.print(data[i]);
      Serial.print("\t");
    }
    Serial.println("");
  
    save_frame(data, ix);
    
    ix = 0;
    delay(SAMPLING_MS);
  }
  
  return 0;
}

void init_setup(void){
  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  
  as7262_init(&as7262);
  dht20_init(&dht20);

  delay(2000);
}
/*
void setup() {
  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  
  as7262_init(&as7262);
  dht20_init(&dht20);

  delay(2000);
}

void loop() {

  uint8_t data[32] = {0};
  uint8_t ix = 0;

  while(true){
    Serial.println("Printing indexes: ");
    ix += as7262.sread(&as7262, data + ix);
    Serial.println(ix);
    ix += dht20.sread(&dht20, data + ix);
    Serial.println(ix);
    Serial.println("Printing data[32]: ");
    for(int i = 0; i < 32; i++){
      Serial.print(data[i]);
      Serial.print("\t");
    }
    Serial.println("");
  
    save_frame(data, ix);
    
    ix = 0;
    delay(SAMPLING_MS);
  }
}
/*
void sd_init(){
  
}
*/

void save_frame(uint8_t* data, uint8_t len){

  for(int i = 0; i < len; i++){
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(data[i]);
  }
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File dataFile = SD.open("datalog.bin", FILE_WRITE);
  
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

bool as7262_init(Sensor_t* sens){
  sens->sread = &as7262_read;
}

uint8_t as7262_read(Sensor_t* sens, uint8_t* data){

  float data_float[6] = {13.7, 18.4, 12.6, 37.12, 53.9, 21.495};

  data_float_bytes fb;

  for(int i = 0; i < 6; i++){
    fb.value = data_float[i];
    for(int j = 0; j < sizeof(float); j++){
      data[i * sizeof(float) + j] = fb.bytes[j];
    }
  }
  return 6 * sizeof(float);
}

bool dht20_init(Sensor_t* sens){
  DFRobot_DHT20 dht20_sensor;
  sens->sensor_mod = (void*)&dht20_sensor;
  
  sens->sread = &dht20_read;

  dht20_sensor.begin();
}

uint8_t dht20_read(Sensor_t* sens, uint8_t* data){
  DFRobot_DHT20* dht20_sensor = (DFRobot_DHT20*)sens->sensor_mod;
  data_float_bytes temp;
  data_float_bytes rh;
  temp.value = dht20_sensor->getTemperature();
  rh.value = dht20_sensor->getHumidity();
  
  for(int i = 0; i < sizeof(float); i++){
    data[i] = temp.bytes[i];
    data[i + sizeof(float)] = rh.bytes[i];
    /*
    Serial.print(i);
    Serial.print("\t");
    Serial.print(data[i]);
    Serial.print("\t");
    Serial.println(data[i + sizeof(float)]);
    */
  }
  /*
  Serial.print("temperature : ");
  Serial.print(temp.value);
  Serial.print("\thumidity : ");
  Serial.println(rh.value);
*/
  return sizeof(float) * 2;
  
}
