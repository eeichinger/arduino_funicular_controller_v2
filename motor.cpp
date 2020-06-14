#include "motor.h"
#include <Arduino.h>

#define PIN_HALL1 4
#define PIN_HALL2 5

/**
    Rotary Encoder sensing rope speed + dir
*/
// create LegoHallRotEncoder
LegoHallRotEncoder rotEnc = LegoHallRotEncoder(PIN_HALL1, PIN_HALL2);

static void nothing(void) {
}

static void (*cb)(void) = nothing;

ISR(PCINT2_vect) {
  cb();
}

static void myISR() {
  static byte oldRotEncState = 0x0;
  // see https://arduino.stackexchange.com/a/12958 how to handle pin change interrupts
  byte newRotEncState = PIND & (bit(PIN_HALL1) | bit(PIN_HALL2));
  if (newRotEncState != oldRotEncState) {
    LegoHallRotEncoder::rotenc_changed();
  }
  oldRotEncState = newRotEncState;
}

extern void motor_init() {
  cb = myISR;

  cli(); // disable interrupts
  PCICR |= 0b00000100; // turn on pin change irq on port d
  PCMSK2 |= (bit(PIN_HALL1) | bit(PIN_HALL2));    // turn on pins PD4 & PD5, PCINT20 & PCINT21 (=Arduino pins D4, D5) // (bit(PIN_HALL1)|bit(PIN_HALL2));
  sei(); // reenable interrupts
}
