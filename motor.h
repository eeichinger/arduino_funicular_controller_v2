#ifndef MOTOR_H
#define MOTOR_H

#include <LegoHallRotEncoder.h>

extern LegoHallRotEncoder rotEnc;

extern void motor_init();
extern void motor_reset();
extern int16_t motor_speed();
extern void motor_setspeed(int16_t speed);

#endif
