#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"

// Function declarations
void initializeSensors();
void powerUpSensors();
void powerDownSensors();
SensorReadings readAllSensors();
float readNTCTemperature();
float readLDRLightIntensity();
float readSoilMoisture();

#endif