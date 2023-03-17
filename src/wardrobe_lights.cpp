#include <Arduino.h>

#define STRIP_SHELF_PIN 4     // пин полки
#define STRIP_WALL_PIN 3    // пин стенки
#define NUMLEDS_SHELF 20
#define WALL_STRIPS 1
#define WALL_STRIP_LEDS 1
#define TIMEOUT 5
#define PIR_SENSOR 7
#define BRIGHTNESS 150U
#define SHELF_COLOR 0x4374e8
#define WALL_COLOR 0xE91627
#define SHELF_EFFECT_SPEED 1
#define WALL_EFFECT_SPEED 3

static uint32_t timeoutCounter;
static bool active = true;

static const byte brightness_delta = 1;
static byte shelf_skip_counter = SHELF_EFFECT_SPEED;
static uint8_t shelf_counter_brightness = 0;

static uint8_t wall_counter_brightness = 0;
static byte wall_skip_counter = WALL_EFFECT_SPEED;

#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
microLED<NUMLEDS_SHELF, STRIP_SHELF_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip_shelf;
microLED<WALL_STRIPS * WALL_STRIP_LEDS, STRIP_WALL_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip_wall;

void brightness_counter(uint8_t effect_speed, uint8_t& skip_counter, uint8_t& brightness_counter){
  if(skip_counter > 0) {
    skip_counter--;
    return;
  }
  skip_counter = effect_speed;
  if(active){
    if(brightness_counter < BRIGHTNESS) {
      if(255 - brightness_counter > brightness_delta) {
        brightness_counter += brightness_delta;
      } else {
        brightness_counter += 255 - brightness_counter;
      }
    }
  } else {
    if(brightness_counter > 0) {
      if(brightness_counter > brightness_delta) {
        brightness_counter -= brightness_delta;
      } else {
        brightness_counter = 0;
      }
    }
  }
}

void shelf_filler() {
  brightness_counter(SHELF_EFFECT_SPEED, shelf_skip_counter, shelf_counter_brightness);
  strip_shelf.setBrightness(shelf_counter_brightness);
  strip_shelf.show();
}

void wall_filler() {
  brightness_counter(WALL_EFFECT_SPEED, wall_skip_counter, wall_counter_brightness);
  strip_wall.setBrightness(wall_counter_brightness);
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
  wall_filler();
  delay(10);
}

void setup() {
  pinMode(PIR_SENSOR, INPUT);
  strip_shelf.fill(SHELF_COLOR);
  strip_shelf.setBrightness(0);
  strip_wall.fill(WALL_COLOR);
}

void loop() {
  loop_iteration();
}