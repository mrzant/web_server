#ifndef _SENSOR_H
#define _SENSOR_H

#define PIN 4
#define CHECKSUM 0
#define SUCCESS 1
#define TIMEOUT 2

char temp_hum[50];

int sensor(void);

#endif
