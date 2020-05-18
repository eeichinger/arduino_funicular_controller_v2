/*
  hall rotary encoder

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#include "LegoHallRotEncoder.h"

const int PIN_HALL1 = 2;
const int PIN_HALL2 = 3;

LegoHallRotEncoder rotEnc = LegoHallRotEncoder(PIN_HALL1, PIN_HALL2);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
}


// the loop function runs over and over again forever
void loop() {
//  rotEnc.printDebug();
  Serial.print("dir:");
  Serial.print(rotEnc.dir());
  Serial.print("counter:");
  Serial.print(rotEnc.counter());
  Serial.println();  

  if (rotEnc.counter() > 50) rotEnc.resetCounter();
  //  noInterrupts();
  //  hall_state = counter;
  //  counter = 0;
  //  interrupts();
  //  Serial.println(hall_state);
  delay(100);                       // wait a bit
}
