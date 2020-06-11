/*
   Funicular Controller V2


*/

#include <LegoHallRotEncoder.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
//#define PN532_SCK  (13)
//#define PN532_MISO (12)
//#define PN532_MOSI (11)
#define PN532_SS   (9)
#define PN532_IRQ   (2)
#define PN532_SS_2   (10)
#define PN532_IRQ_2   (3)

const int PIN_HALL1 = 4;
const int PIN_HALL2 = 5;

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

// create LegoHallRotEncoder
LegoHallRotEncoder rotEnc = LegoHallRotEncoder(PIN_HALL1, PIN_HALL2);

ISR(PCINT2_vect) {
  static byte oldRotEncState = 0x0;

  // see https://arduino.stackexchange.com/a/12958 how to handle pin change interrupts
  byte newRotEncState = PIND & 0b00110000;  //(bit(4)|bit(5));
  if (newRotEncState != oldRotEncState) {
    LegoHallRotEncoder::rotenc_changed();
  }
  oldRotEncState = newRotEncState;
}

void initRFIDReader(Adafruit_PN532& rfidReader, String name, uint8_t irq_pin) {
  Serial.print("Init PN532 Reader:"); Serial.println(name);
  rfidReader.begin();
  uint32_t versiondata;
  versiondata = rfidReader.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board InStation");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);
  // configure board to read RFID tags
  rfidReader.SAMConfig();

  rfidReader.enableAsync(irq_pin);
  // start async listening mode
  rfidReader.beginReadPassiveTargetID(PN532_MIFARE_ISO14443A);
}

// the setup function runs once when you press reset or power the board
void setup() {
  cli(); // disable interrupts
  PCICR |= 0b00000100; // turn on pin change irq on port d
  PCMSK2 |= 0b00110000;    // turn on pins PD4 & PD5, PCINT20 & PCINT21 (=Arduino pins D4, D5)
  sei(); // reenable interrupts

  Serial.begin(115200);
  initRFIDReader(nfc_beforeStation, "BeforeStation", PN532_IRQ);
  initRFIDReader(nfc_inStation, "InStation", PN532_IRQ_2);
}

int8_t detectCar(Adafruit_PN532& nfc) { // carid = -1, 0=no car, 1
  byte success = false;
  byte uid[] = "\0\0\0\0\0\0\0"; // Buffer to store the returned UID
  byte uidLength;                // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // fetch result if IRQ line indicates that there is one
  success = nfc.completeReadPassiveTargetID(uid, &uidLength);
  int8_t car_id = 0;
  if (success) {
    uint16_t curcar =  (uid[1] << 8) | uid[2];
    Serial.print("car id:");
    Serial.print(curcar, HEX);
    Serial.println("");

//    Serial.print(carIds[0][0], HEX);
//    Serial.print(carIds[0][1], HEX);
    //    Serial.print(carIds[1][0], HEX);
    //    Serial.print(carIds[1][1], HEX);
//    Serial.println("");
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
//  rotEnc.printDebug();
//  Serial.print("dir:");
//  Serial.print(rotEnc.dir());
//  Serial.print("counter:");
//  Serial.print(rotEnc.counter());
//  Serial.println();

  if (rotEnc.counter() > 50) rotEnc.resetCounter();

  int8_t car_inStation = detectCar(nfc_inStation);
  if (car_inStation != 0) {
    Serial.print("car inStation:"); Serial.println(car_inStation);
  }
  int8_t car_beforeStation = detectCar(nfc_beforeStation);
  if (car_beforeStation != 0) {
    Serial.print("car beforeStation:"); Serial.println(car_beforeStation);
  }

  Serial.flush();
  delay(50);                       // wait a bit
}
