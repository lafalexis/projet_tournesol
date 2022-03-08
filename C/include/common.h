#define FRAME_SIZE 46

typedef struct Sensor{
	void* read;
	uint8_t* data;
	uint8_t data_length;
} Sensor_t;