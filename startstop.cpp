#include "startstop.h"
#include <Arduino.h>

#define DEBUG

/**
  Start/Stop Button
 */
#define STARTSTOP_BTN A2

void startstop_init() {
  pinMode(STARTSTOP_BTN, INPUT_PULLUP);
}

bool startstop_isButton_pressed() {
  uint8_t btnPressed = digitalRead(STARTSTOP_BTN);
  if (btnPressed == 0) {
    return true;
  }
  return false;
}
