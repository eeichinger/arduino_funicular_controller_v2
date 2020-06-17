#include "startstop.h"
#include <Arduino.h>

//#define DEBUG

/**
  Start/Stop Button
 */
#define STARTSTOP_BTN A2

void startstop_init() {
  pinMode(STARTSTOP_BTN, INPUT_PULLUP);
}

int lastState = 0;

bool startstop_isButton_pressed() {
  uint8_t btnPressed = digitalRead(STARTSTOP_BTN);
  if (btnPressed==0 && lastState==0) {
    // wait until it's released again    
    return false;
  }
  lastState = btnPressed;
  return (btnPressed == 0);
}
