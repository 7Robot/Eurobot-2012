

#include <timers.h>
#include <p18f2550.h>



#ifndef _SERVO_H
#define _SERVO_H

void OpenServo(int count);
void WriteServo(int n, int angle);
void SwitchServo(int n);
void InterruptServo();

#endif
