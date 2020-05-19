/*
 * Funicular Controller V2
 * 
 * 
*/

#include <LegoHallRotEncoder.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <MFRC522.h>

#define RC522_SS 9         // Configurable, see typical pin layout above
#define RC522_RST 8         // Configurable, see typical pin layout above

// If using the breakout with SPI, define the pins for SPI communication.
//#define PN532_SCK  (13)
//#define PN532_MISO (12)
//#define PN532_MOSI (11)
#define PN532_SS   (10)

const int PIN_HALL1 = 2;
const int PIN_HALL2 = 3;

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
Adafruit_PN532 nfc(PN532_SS);

// Create MFRC522 instance
MFRC522 mfrc522(RC522_SS, RC522_RST);

// create LegoHallRotEncoder
LegoHallRotEncoder rotEnc = LegoHallRotEncoder(PIN_HALL1, PIN_HALL2);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);

  Serial.println("Init PN532");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
//    while (1); // halt
  } else {
    // Got ok data, print it out!
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);  
    // configure board to read RFID tags
    nfc.SAMConfig();    
  }
  
  Serial.println("Init RC522");  
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_SetAntennaGain(MFRC522::PCD_RxGain::RxGain_max);
//  mfrc522.PCD_AntennaOn();

  delay(10);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details  
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
  poll_rfid_readers();
  
  delay(100);                       // wait a bit
}

void dump_uid(int len, const byte uid[]) {
  for (byte i = 0; i < len; i++) {
    Serial.print(uid[i] < 0x10 ? " 0" : " ");
    Serial.print(uid[i], HEX);
  }
}

void determine_car_in_station() {
  Serial.print("checking rc522...");
  mfrc522.PCD_AntennaOn();
  delay(5);
  if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("detected car");
    dump_uid(7, mfrc522.uid.uidByte);
    Serial.println();
//    car_in_station = check_car_in_station(mfrc522.uid.uidByte);
  }
  mfrc522.PCD_AntennaOff();
  Serial.println("...done");
//  mfrc522.PICC_HaltA();
//  return car_in_station;
}

void poll_rfid_readers(void) {
  Serial.println("polling...");
//  delay(200);
  
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  determine_car_in_station();
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    
    // Start with block 4 (the first block of sector 1) since sector 0
    // contains the manufacturer data and it's probably better just
    // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
    
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
    
        // If you want to write something to block 4 to test with, uncomment
    // the following line and this text should be read back in a minute
        //memcpy(data, (const uint8_t[]){ 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0 }, sizeof data);
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);
    
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Block 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");
      
          // Wait a bit before reading the card again
          delay(1000);
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
    
      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");
    
        // Wait a bit before reading the card again
//        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
}
