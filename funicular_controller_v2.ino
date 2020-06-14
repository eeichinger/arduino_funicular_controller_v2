/*
   Funicular Controller V2


*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PN532.h>
#include <L293.h>

#include "display.h"
#include "nfc.h"
#include "motor.h"

#define DEBUG

/**
  Start/Stop Button
 */
#define STARTSTOP_BTN A2

/**
    Motor
*/
// give a name to the pins that we use
const int speedPin = 6;    // that is the pin that we use to control the motor's speed
const int forwardPin = 7; // this is the pin that we use to tell the motor to go forward
const int reversePin = 8; // this is the pin that we use to tell the motor to go reverse

L293 motor( speedPin, forwardPin, reversePin );

/**
    NFC Sensors
*/
// If using the breakout with SPI, define the pins for SPI communication.
//#define PN532_SCK  (13)
//#define PN532_MISO (12)
//#define PN532_MOSI (11)
#define PN532_SS   (9)
#define PN532_IRQ   (2)
#define PN532_SS_2   (10)
#define PN532_IRQ_2   (3)

const uint16_t carIds[2][2] = {
  { 0xC1F4, 0xC7F4 }, // car -1
  { 0xCCF5, 0xD2F5 } // car 1
};

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
Adafruit_PN532 nfc_beforeStation(PN532_SS);
Adafruit_PN532 nfc_inStation(PN532_SS_2);

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(STARTSTOP_BTN, INPUT_PULLUP);
  
  Serial.begin(115200);
  Serial.println(F("BEGIN"));

  Serial.println(bit(4)|bit(5), BIN);
  
  motor.forceStop(200); // stop the motor

  initRFIDReader(nfc_beforeStation, "BeforeStation", PN532_IRQ);
  initRFIDReader(nfc_inStation, "InStation", PN532_IRQ_2);

  motor_init();
  display_init();
  
  Serial.println(F("setup complete"));
}


static byte uid[] = "\0\0\0\0\0\0\0"; // Buffer to store the returned UID
static byte uidLength;                // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

int8_t detectCar(Adafruit_PN532& nfc) { // carid = -1, 0=no car, 1
  byte success = false;

  // fetch result if IRQ line indicates that there is one
  success = nfc.completeReadPassiveTargetID(uid, &uidLength);
  int8_t car_id = 0;
  if (success) {
    uint16_t curcar =  (uid[1] << 8) | uid[2];
#ifdef DEBUG
    Serial.print("car id:");
    Serial.print(curcar, HEX);
    Serial.println("");
#endif
    if ( carIds[0][0] == curcar || carIds[0][1] == curcar ) {
      car_id = -1;
    }
    if (carIds[1][0] == curcar || carIds[1][1] == curcar) {
      car_id = 1;
    }
    // put back into listening mode
    nfc.beginReadPassiveTargetID(PN532_MIFARE_ISO14443A);
  }

  return car_id;
}

// the loop function runs over and over again forever
void loop() {
  uint8_t btnPressed = digitalRead(STARTSTOP_BTN);
  if (btnPressed == 0) {
#ifdef DEBUG
    Serial.println("btnPressed=YES");
#endif      
  }
  
  int8_t car_inStation = detectCar(nfc_inStation);
  if (car_inStation != 0) {
#ifdef DEBUG
    Serial.print("car inStation:"); Serial.println(car_inStation);
#endif  
  }

  int8_t car_beforeStation = detectCar(nfc_beforeStation);
  if (car_beforeStation != 0) {
#ifdef DEBUG
    Serial.print("car beforeStation:"); Serial.println(car_beforeStation);
#endif  
  }

#ifdef DEBUG
  Serial.flush();
#endif  
  display_status(rotEnc.counter(), car_inStation, car_beforeStation);
}
