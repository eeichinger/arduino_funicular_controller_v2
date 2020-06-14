/*
   Funicular Controller V2


*/
#include "startstop.h"
#include "display.h"
#include "nfc.h"
#include "motor.h"

#define DEBUG

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial.println(F("BEGIN"));

  startstop_init();
  nfc_init();
  motor_init();
  display_init();
  
  Serial.println(F("setup complete"));
}


// the loop function runs over and over again forever
void loop() {
  bool startStopPressed = startstop_isButton_pressed();  
  int8_t car_inStation = nfc_detectCar(nfc_inStation);
  int8_t car_beforeStation = nfc_detectCar(nfc_beforeStation);

  display_status(startStopPressed, rotEnc.counter(), car_inStation, car_beforeStation);
}
