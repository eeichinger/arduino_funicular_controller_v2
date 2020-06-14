#ifndef NFC_H
#define NFC_H

#include <stdint.h>
#include <Adafruit_PN532.h>

extern Adafruit_PN532 nfc_beforeStation;
extern Adafruit_PN532 nfc_inStation;

extern void nfc_init();
extern int8_t nfc_detectCar(Adafruit_PN532& nfc);

#endif
