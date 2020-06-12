/*
   Funicular Controller V2


*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PN532.h>
#include <L293.h>
#include <LegoHallRotEncoder.h>

#define DEBUG

/**
    DISPLAY
*/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

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

/**
    Rotary Encoder sensing rope speed + dir
*/
// create LegoHallRotEncoder
LegoHallRotEncoder rotEnc = LegoHallRotEncoder(PIN_HALL1, PIN_HALL2);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static void nothing(void) {
}

void (*cb)(void) = nothing;

ISR(PCINT2_vect) {
  cb();
}

static void myISR() {
  static byte oldRotEncState = 0x0;
  // see https://arduino.stackexchange.com/a/12958 how to handle pin change interrupts
  byte newRotEncState = PIND & 0b00110000;  //(bit(4)|bit(5));
  if (newRotEncState != oldRotEncState) {
    LegoHallRotEncoder::rotenc_changed();
  }
  oldRotEncState = newRotEncState;
}

void initRFIDReader(Adafruit_PN532& rfidReader, String name, uint8_t irq_pin) {
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

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  Serial.println(F("BEGIN"));

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    Serial.flush();
    for (;;); // Don't proceed, loop forever
  }

  motor.forceStop(200); // stop the motor

  initRFIDReader(nfc_beforeStation, "BeforeStation", PN532_IRQ);
  initRFIDReader(nfc_inStation, "InStation", PN532_IRQ_2);

  cb = myISR;

  cli(); // disable interrupts
  PCICR |= 0b00000100; // turn on pin change irq on port d
  PCMSK2 |= 0b00110000;    // turn on pins PD4 & PD5, PCINT20 & PCINT21 (=Arduino pins D4, D5)
  sei(); // reenable interrupts

//   Show initial display buffer contents on the screen --
//   the library initializes this with an Adafruit splash screen.
  display.display();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  Serial.println(F("setup complete"));
}

static uint32_t time = millis();

void testprint(int counter, int8_t car_inStation, int8_t car_beforeStation) {
  display.clearDisplay();

  display.setCursor(0, 0);     // Start at top-left corner
//  display.println("text-performance test");
  //  for (int i = 0; i < 255; i++) {
  ////    display.setCursor(0, 9);
  ////    display.print("A");
  //    display.display();
  //
  //  }
  time = millis() - time;
  display.setCursor(0, 9);
  display.print(F("loop time:"));
  display.print(time);
  display.print("ms");

  display.setCursor(0, 18);
  display.print(F("counter:"));
  display.print(counter);
  
  display.setCursor(0, 27);
  display.print(F("car_inStation:"));
  display.print(car_inStation);

  display.setCursor(0, 36);
  display.print(F("car_beforeStation:"));
  display.print(car_beforeStation);

  display.display();

  time = millis();
  //  delay(2000);
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
#ifdef DEBUG
    rotEnc.printDebug();
  //  Serial.print("dir:");
  //  Serial.print(rotEnc.dir());
  //  Serial.print("counter:");
  //  Serial.print(rotEnc.counter());
  //  Serial.println();
#endif    

  if (rotEnc.counter() > 50) rotEnc.resetCounter();

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
  testprint(rotEnc.counter(), car_inStation, car_beforeStation);
}
