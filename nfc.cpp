#include "nfc.h"
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>

//#define DEBUG

/**
    NFC Sensors

    using hardware SPI
*/
// NFC beforeStation
#define PN532_SS   (9)
#define PN532_IRQ   (2)

// NFC inStation
#define PN532_SS_2   (10)
#define PN532_IRQ_2   (3)

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
Adafruit_PN532 nfc_beforeStation(PN532_SS);
Adafruit_PN532 nfc_inStation(PN532_SS_2);

static void initRFIDReader(Adafruit_PN532& rfidReader, String name, uint8_t irq_pin) {
  Serial.print(F("Init PN532 Reader:")); Serial.println(name);
  rfidReader.begin();
  uint32_t versiondata;
  versiondata = rfidReader.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(F("Didn't find PN53x board InStation"));
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print(F("Found chip PN5")); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print(F("Firmware ver. ")); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print(F(".")); Serial.println((versiondata >> 8) & 0xFF, DEC);
  // configure board to read RFID tags
  rfidReader.SAMConfig();

  rfidReader.enableAsync(irq_pin);
  // start async listening mode
  rfidReader.beginReadPassiveTargetID(PN532_MIFARE_ISO14443A);
}

static const uint16_t carIds[2][2] = {
  { 0xC1F4, 0xC7F4 }, // car -1
  { 0xCCF5, 0xD2F5 } // car 1
};

static byte uid[] = "\0\0\0\0\0\0\0"; // Buffer to store the returned UID
static byte uidLength;                // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

extern void nfc_init() {
  initRFIDReader(nfc_beforeStation, "BeforeStation", PN532_IRQ);
  initRFIDReader(nfc_inStation, "InStation", PN532_IRQ_2);  
}

extern int8_t nfc_detectCar(Adafruit_PN532& nfc) { // carid = -1, 0=no car, 1
  byte success = false;

  // fetch result if IRQ line indicates that there is one
  success = nfc.completeReadPassiveTargetID(uid, &uidLength);
  int8_t car_id = CAR_NONE;
  if (success) {
    uint16_t curcar =  (uid[1] << 8) | uid[2];
#ifdef DEBUG
    Serial.print("car id:");
    Serial.print(curcar, HEX);
    Serial.println("");
#endif
    if ( carIds[0][0] == curcar || carIds[0][1] == curcar ) {
      car_id = CAR_B;
    }
    if (carIds[1][0] == curcar || carIds[1][1] == curcar) {
      car_id = CAR_A;
    }
    // put back into listening mode
    nfc.beginReadPassiveTargetID(PN532_MIFARE_ISO14443A);
  }

  return car_id;
}
