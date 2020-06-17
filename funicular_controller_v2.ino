/*
   Funicular Controller V2


*/
#include "startstop.h"
#include "display.h"
#include "nfc.h"
#include "motor.h"
#include "state.h"

//#define DEBUG

void(* resetFunc) (void) = 0; //declare reset function at address 0

const int MAX_SPEED = 150;

State state;
StateHandler stateHandlers[STOPPED];
StateContext ctx;

// start state
State handle_uninitialized(StateContext &ctx) {
  if (ctx.startStopPressed) {
    motor_setspeed(-MAX_SPEED);
    return State::CAR_IN_FLIGHT;
  }
  return ctx.state;
}

State handle_car_in_station(StateContext &ctx) {
  motor_reset();
  if (ctx.startStopPressed) {
    motor_setspeed(MAX_SPEED * ctx.car_inStation);
    return State::CAR_IN_FLIGHT;
  }
  return ctx.state;
}

State handle_car_in_flight(StateContext &ctx) {
  if (ctx.startStopPressed) { // emergency stop
    motor_setspeed(0);
    return State::UNINITIALIZED;
  }
  int speed = motor_speed();
  int dir = speed / abs(speed);
  if (ctx.car_inStation == -dir) {
    motor_setspeed(0);
    return State::CAR_IN_STATION;
  }
  if (ctx.car_beforeStation == -dir) {
    motor_setspeed(MAX_SPEED / 2 * dir);
  }
  return ctx.state;
}

// the setup function runs once when you press reset or power the board
void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println(F("BEGIN"));
#endif

  stateHandlers[State::UNINITIALIZED] = handle_uninitialized;
  stateHandlers[State::CAR_IN_FLIGHT] = handle_car_in_flight;
  stateHandlers[State::CAR_IN_STATION] = handle_car_in_station;

  startstop_init();
  nfc_init();
  motor_init();
  display_init();

#ifdef DEBUG
  Serial.println(F("setup complete"));
#endif
}

void readStateContext(StateContext &ctx) {
  ctx.state = state;
  ctx.startStopPressed = startstop_isButton_pressed();
  ctx.distance = rotEnc.counter();
  ctx.speed = motor_speed();
  ctx.car_inStation = nfc_detectCar(nfc_inStation);
  ctx.car_beforeStation = nfc_detectCar(nfc_beforeStation);
}

// the loop function runs over and over again forever
void loop() {
  readStateContext(ctx);
  state = stateHandlers[ctx.state](ctx);
  display_status(ctx);
  delay(20);
  // if (uptime > 24h) resetFunc(); //call reset 
}
