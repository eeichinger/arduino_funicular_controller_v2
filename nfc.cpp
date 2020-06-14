#include "nfc.h"

#define DEBUG

extern void initRFIDReader(Adafruit_PN532& rfidReader, String name, uint8_t irq_pin) {
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
