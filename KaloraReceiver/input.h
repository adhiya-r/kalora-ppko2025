#ifndef INPUT_H
#define INPUT_H

#include "config.h"

// Function declarations
void initializeJoystick();
void handleJoystickInput();
void handleMenuSelection();

// Global variables
extern int joystickCenterX;
extern int joystickCenterY;
extern unsigned long lastJoystickRead;

#endif