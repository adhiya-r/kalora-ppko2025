#ifndef SLEEP_H
#define SLEEP_H

#include "config.h"

// Global variables
extern int bootCount;

// Function declarations
void print_wakeup_reason();
void enterDeepSleep();

#endif