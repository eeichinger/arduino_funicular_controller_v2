#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

extern void display_init();

extern void display_status(int counter, int8_t car_inStation, int8_t car_beforeStation);

#endif
