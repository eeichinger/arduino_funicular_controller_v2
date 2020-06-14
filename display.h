#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "state.h"

extern void display_init();

extern void display_status(StateContext &ctx);

#endif
