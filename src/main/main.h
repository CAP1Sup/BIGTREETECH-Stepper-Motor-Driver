#ifndef __MAIN_H__
#define __MAIN_H__

#include "Arduino.h"
#include "stdint.h"
#include "config.h"
#include "motor.h"

extern StepperMotor motor;

void setup();
void loop();

void blink();

#endif
