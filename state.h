#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

typedef enum {
  UNINITIALIZED = 0
  , INITIALIZING
  , CAR_IN_STATION
  , CAR_IN_FLIGHT
  , STOPPED  
} State;


typedef struct {
  State state;
  bool startStopPressed;
  int16_t speed;
  int distance;
  int8_t car_inStation;
  int8_t car_beforeStation;
} StateContext;

// handle state and return next state
typedef State (*StateHandler)(StateContext&);

#endif
