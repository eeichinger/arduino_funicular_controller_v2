#include "display.h"
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>


#define DEBUG

/**
    DISPLAY
*/
static SSD1306AsciiAvrI2c display;

static uint32_t time = millis();

void display_init() {
  display.begin(&Adafruit128x64, 0x3C);
  display.setFont(Stang5x7);  
  display.setContrast(255);  
  display.setLetterSpacing(0);
  
  display.setCursor(0, 0);
  display.print(F("xx   refresh (ms)"));

  display.setCursor(0, 1);
  display.print(F("xx   counter"));
  
  display.setCursor(0, 2);
  display.print(F("xx   car_inStation"));

  display.setCursor(0, 3);
  display.print(F("xx   car_beforeStation"));  

  time = millis();
}

void display_status(int counter, int8_t car_inStation, int8_t car_beforeStation) {
  uint32_t duration = millis() - time;
  time = millis();
  
  display.setCursor(0, 0);
  display.print(duration);
  if (duration<10) {
    display.print(F("  "));
  } else if (duration < 100) {
    display.print(F(" "));    
  }
  display.setCursor(0, 1);
  display.print(counter);
  if (counter>0) {
    display.print(F(" "));    
  }
  if (abs(counter)<10) {
    display.print(F("  "));
  } else if (abs(counter) < 100) {
    display.print(F(" "));    
  }
  
  display.setCursor(0, 2);
  display.print(car_inStation);
  if (car_inStation>=0) {
    display.print(F(" "));        
  }

  display.setCursor(0, 3);
  display.print(car_beforeStation);
  if (car_beforeStation>=0) {
    display.print(F(" "));        
  }
}
