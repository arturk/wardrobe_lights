#include <Arduino.h>
// простейшие динамические эффекты
// сначала ознакомься с примером microLED_guide !!!

#define STRIP_SHELF_PIN 9     // пин полки
#define STRIP_WALL_PIN 8    // пин стенки
#define NUMLEDS_SHELF 20
#define NUMLEDS_WALL 4
#define TIMEOUT 5
#define PIR_SENSOR 12
#define BRIGHTNESS 150U
#define SHELF_COLOR 0x4374e8
#define WALL_COLOR 0x262dc5
#define SHELF_EFFECT_SPEED 20
#define WALL_EFFECT_SPEED 1

static uint32_t timeoutCounter;
static int shelf_counter = 0;
static int shelf_counter_brightness = 0;
static byte running_dots_counter = 0;
static bool active = false;

#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
microLED<NUMLEDS_SHELF, STRIP_SHELF_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip_shelf;
microLED<NUMLEDS_WALL, STRIP_WALL_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip_wall;

void shelf_filler() {
  for(byte i=0; i<SHELF_EFFECT_SPEED; i++){
    if(active){
      if(shelf_counter < NUMLEDS_SHELF){
        if(shelf_counter_brightness < BRIGHTNESS) {
          shelf_counter_brightness++;
        } else {
          shelf_counter++;
          shelf_counter_brightness = 0;
        }
        strip_shelf.set(shelf_counter, mWheel(SHELF_COLOR, shelf_counter_brightness));
      }
    } else {
      if(shelf_counter > 0){
        if(shelf_counter_brightness > 0) {
          shelf_counter_brightness--;
        } else {
          shelf_counter_brightness = BRIGHTNESS;
          shelf_counter--;
        }
        strip_shelf.set(shelf_counter, mWheel(SHELF_COLOR, shelf_counter_brightness));
      }
    }
    delay(5);
  }
}

void filler2() {
  strip_wall.clear(); // clear
  if(active) { // fill strip to led counter
    if (running_dots_counter == NUMLEDS_WALL) {
      strip_wall.leds[0] = mRed;
      running_dots_counter = 0;
    }
    strip_wall.leds[running_dots_counter] = mWhite;
    running_dots_counter++;
  }
  strip_wall.show();
}

void check_sensor_activity() {
  if (digitalRead(PIR_SENSOR)) {
    timeoutCounter = millis();
    active = true;
  }
  if((millis() - timeoutCounter) >= (TIMEOUT * 1000L)) {
    active = false;
  }
}

void loop_iteration() {
  check_sensor_activity();
  shelf_filler();
  filler2();
}

void setup() {
  pinMode(PIR_SENSOR, INPUT);
}

void loop() {
  loop_iteration();
}