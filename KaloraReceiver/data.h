#ifndef DATA_H
#define DATA_H

#include "config.h"

// Function declarations
void initializeData();
SensorData* getLatestData();
RegisteredSender* getSendersArray();
int getSenderCount();

#endif