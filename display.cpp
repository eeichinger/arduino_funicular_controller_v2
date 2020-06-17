#include "display.h"
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#include <math.h>

//#define DEBUG

/**
    DISPLAY
*/
static SSD1306AsciiAvrI2c display;

static unsigned long time = millis();

void display_init() {
  display.begin(&Adafruit128x64, 0x3C);
  display.setFont(Stang5x7);
  display.setContrast(255);
  display.setLetterSpacing(0);

  display.setCursor(0, 0);
  display.println(F("xxxxxxx uptime (s)"));
  display.println(F("xxxx refresh (ms)"));
  display.println(F("xxxx state"));
  display.println(F("xxxx speed"));
  display.println(F("xxxx distance"));
  display.println(F("xxxx car_inStation"));
  display.println(F("xxxx car_beforeStation"));

  time = millis();
}

void printPadded(int8_t val) {
  if (val < -99) {
    // noop
  } else if (val < -9) {
    display.print(F(" "));
  } else if (val < 0) {
    display.print(F("  "));
  } else if (val < 10) {
    display.print(F("   "));
  } else if (val < 100) {
    display.print(F("  "));
  } else {
    display.print(F(" "));
  }
  display.println(val);
}

const String s_spaces[] = {
  ""
  , " "
  , "  "
  , "   "
  , "    "
  , "     "
  , "      "
  , "       "
  , "        "
  , "         "
  , "          "
};

void printPadded(long val, int len) {
  int l = log10(abs(val)) + 1;
  int spaces = max(0, len - l);
  if (val<0 && spaces>0) spaces -=1;
  display.print(s_spaces[spaces]);
  display.println(val);
}

void display_status(StateContext &ctx) {
  uint8_t refreshMillis = millis() - time;
  time = millis();

#ifdef DEBUG
  Serial.print(F("uptime (s): ")); Serial.print(time / 1000);
  Serial.print(F("\trefreshMs: ")); Serial.print(refreshMillis);
  Serial.print(F("\tstate: ")); Serial.print(ctx.state);
  Serial.print(F("\tspeed: ")); Serial.print(ctx.speed);
  Serial.print(F("\tdistance: ")); Serial.print(ctx.distance);
  Serial.print(F("\tbeforeStation: ")); Serial.print(ctx.car_beforeStation);
  Serial.print(F("\tinStation: ")); Serial.print(ctx.car_inStation);
  Serial.print(F("\tstartStop: ")); Serial.print(ctx.startStopPressed);
  Serial.println();
  Serial.flush();
#endif

  display.setCursor(0, 0);

  printPadded(time/1000, 7);
  printPadded(refreshMillis);
  printPadded(ctx.state);
  printPadded(ctx.speed);
  printPadded(ctx.distance);
  printPadded(ctx.car_inStation);
  printPadded(ctx.car_beforeStation);
}
