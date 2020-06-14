#ifndef MOTOR_H
#define MOTOR_H

#include <LegoHallRotEncoder.h>
#include <L293.h>

extern LegoHallRotEncoder rotEnc;
extern L293 motor;

extern void motor_init();

#endif
