#include "motor.h"
#include <Arduino.h>

//#define DEBUG

#define PIN_HALL1 4
#define PIN_HALL2 5

/**
    Motor
*/
// give a name to the pins that we use
const int speedPin = 6;    // that is the pin that we use to control the motor's speed
const int forwardPin = 7; // this is the pin that we use to tell the motor to go forward
const int reversePin = 8; // this is the pin that we use to tell the motor to go reverse

/**
    Rotary Encoder: sensing rope movement, speed + dir
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

static void rotenc_init() {
  cb = myISR;

  cli(); // disable interrupts
  PCICR |= 0b00000100; // turn on pin change irq on port d
  PCMSK2 |= (bit(PIN_HALL1) | bit(PIN_HALL2));    // turn on pins PD4 & PD5, PCINT20 & PCINT21 (=Arduino pins D4, D5) // (bit(PIN_HALL1)|bit(PIN_HALL2));
  sei(); // reenable interrupts
}

extern void motor_init() {
  rotenc_init();

  pinMode( speedPin,  OUTPUT );
  pinMode( forwardPin, OUTPUT );
  pinMode( reversePin, OUTPUT );
}


int16_t currentSpeed = 0;

extern void motor_reset() {
  rotEnc.resetCounter();
  motor_setspeed(0);
}

extern int16_t motor_speed() {
  return currentSpeed;
}

extern void motor_setspeed(int16_t sp) {
  int16_t speed = constrain(sp, -255, 255);
#ifdef DEBUG
  Serial.print(F("applying speed: ")); Serial.print(speed);
#endif
  if ( (speed < 0 && currentSpeed > 0)
       ||   (speed > 0 && currentSpeed < 0)
     ) {
#ifdef DEBUG
    Serial.print(F("; need to break first"));
#endif
    digitalWrite( reversePin, HIGH );
    digitalWrite( forwardPin, HIGH );
    delay(20);
  }
  currentSpeed = speed;
  if (currentSpeed > 0) {
#ifdef DEBUG
    Serial.print(F("; forward:")); Serial.print(speed);
#endif
    digitalWrite( reversePin, LOW );
    digitalWrite( forwardPin, HIGH );
    analogWrite( speedPin , abs(currentSpeed) );
  } else if (currentSpeed < 0) {
#ifdef DEBUG
    Serial.print(F("; reverse:")); Serial.print(speed);
#endif
    digitalWrite( reversePin, HIGH );
    digitalWrite( forwardPin, LOW );
    analogWrite( speedPin, abs(currentSpeed) );
  } else {
#ifdef DEBUG
    Serial.print(F("; stop:")); Serial.print(speed);
#endif
    digitalWrite( reversePin, HIGH );
    digitalWrite( forwardPin, HIGH );
    analogWrite( speedPin , 0 );
  }
#ifdef DEBUG
  Serial.println();
#endif
}
