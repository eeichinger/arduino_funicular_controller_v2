#ifndef NFC_H
#define NFC_H

#include <stdint.h>
#include <Adafruit_PN532.h>

const int CAR_NONE = 0;
const int CAR_A = 1;
const int CAR_B = -1;

extern Adafruit_PN532 nfc_beforeStation;
extern Adafruit_PN532 nfc_inStation;

extern void nfc_init();
extern int8_t nfc_detectCar(Adafruit_PN532& nfc); // return CAR_A, CAR_B or CAR_NONE

#endif
