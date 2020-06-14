#ifndef NFC_H
#define NFC_H

#include <stdint.h>
#include <Adafruit_PN532.h>

extern void initRFIDReader(Adafruit_PN532& rfidReader, String name, uint8_t irq_pin);

#endif
