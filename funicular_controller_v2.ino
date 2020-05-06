/*
  hall rotary encoder

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

const int PIN_HALL1 = 2;
const int PIN_HALL2 = 3;

volatile int counter;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PIN_HALL1, INPUT_PULLUP);
  pinMode(PIN_HALL2, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(PIN_HALL), isr, FALLING);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);  
}

void isr() {
  counter=counter+1;
}

// the loop function runs over and over again forever
void loop() {
  int hall1 = digitalRead(PIN_HALL1);
  int hall2 = digitalRead(PIN_HALL2);
  Serial.print(hall1);  
  Serial.print(" ");
  Serial.println(hall2);
//  noInterrupts();
//  hall_state = counter;
//  counter = 0;
//  interrupts();
//  Serial.println(hall_state);
  delay(200);                       // wait for a second
}
