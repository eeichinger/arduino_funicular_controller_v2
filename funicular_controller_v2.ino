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
const int DATA = PIN_HALL1;
const int CLK = PIN_HALL2;

volatile int hall1;
volatile int hall2;
volatile int prevNextCode;

volatile int counter = 0;
volatile int dir = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(PIN_HALL1, INPUT_PULLUP);
  pinMode(PIN_HALL2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_HALL1), hall1_changed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_HALL2), hall2_changed, CHANGE);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

void hall1_changed() {
  noInterrupts();
  hall1 = digitalRead(PIN_HALL1);
  dir = read_rotary();
  counter += dir;
  interrupts();
}

void hall2_changed() {
  noInterrupts();
  hall2 = digitalRead(PIN_HALL2);
  dir = read_rotary();
  counter += dir;
  interrupts();
}

// A vald CW or CCW move returns 1, invalid returns 0.
int8_t read_rotary() {
  // experimental sequence dir=1: 00 01 11 01 11 10 11 10
  static int8_t rot_enc_table[255] = {    0    };
  // dir +1
  rot_enc_table[B00011101] = 1;
  rot_enc_table[B01110111] = 1;
  rot_enc_table[B11011110] = 1;
  rot_enc_table[B01111011] = 1;
  rot_enc_table[B11101110] = 1;
  rot_enc_table[B10111000] = 1;
  rot_enc_table[B11100001] = 1;
  rot_enc_table[B10000111] = 1;
  // dir -1
  rot_enc_table[B10111011] = -1;
  rot_enc_table[B11101101] = -1;
  rot_enc_table[B10110111] = -1;
  rot_enc_table[B11011101] = -1;
  rot_enc_table[B01110100] = -1;
  rot_enc_table[B11010010] = -1;
  rot_enc_table[B01001011] = -1;
  rot_enc_table[B00101110] = -1;
  
  int prevdir = rot_enc_table[( prevNextCode )];
  prevNextCode <<= 2;
  if (digitalRead(DATA)) prevNextCode |= 0x02;
  if (digitalRead(CLK)) prevNextCode |= 0x01;
  prevNextCode &= 0xff;

  return ( rot_enc_table[( prevNextCode )]);
}

// the loop function runs over and over again forever
void loop() {
  Serial.print(hall1);
  Serial.print(" ");
  Serial.print(hall2);
  Serial.print(" ");
  Serial.print(dir);
  Serial.print(" ");
  Serial.print(prevNextCode, BIN);
  Serial.print(" ");
  Serial.print(prevNextCode);
  Serial.print(" ");
  Serial.print(counter);
  Serial.println("");

  //  noInterrupts();
  //  hall_state = counter;
  //  counter = 0;
  //  interrupts();
  //  Serial.println(hall_state);
  delay(200);                       // wait for a second
}
