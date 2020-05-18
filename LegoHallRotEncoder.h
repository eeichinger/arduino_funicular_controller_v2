#ifndef LegoHallRotEncoder_h
#define LegoHallRotEncoder_h

#include <inttypes.h>

class LegoHallRotEncoder {

  public:
    LegoHallRotEncoder(uint8_t pin1, uint8_t pin2);
    int counter();
    int8_t dir();
    void resetCounter();
    void printDebug();
    
  private:
//    volatile int _hall1 = -1;
//    volatile int _hall2 = -1;
//    volatile int _prevNextCode = 0;

};

#endif
