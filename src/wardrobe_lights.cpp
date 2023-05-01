#include <Arduino.h>

#define STRIP_SHELF_PIN 6     // пин полки
#define STRIP_WALL_PIN 3    // пин стенки
#define NUMLEDS_SHELF 18    // колличество лед секций в полке
#define WALL_STRIPS 7     // колличество полосок в стенке
#define WALL_STRIP_LEDS 5 // колличество лед секций в полоске стенки
#define TIMEOUT 5   // время активности эффекта
#define PIR_SENSOR 10   // пин сенсора движения
#define BRIGHTNESS 150  // яркость эффектов
#define SHELF_COLOR 1300  // цвет эффекта полки
#define SHELF_EFFECT_SPEED 1  // скорость эффекта полки, чем выше значение, тем медленее
#define WALL_EFFECT_SPEED 2   // скорость эффекта стенки, чем выше значение, тем медленее
#define WAVE_SPEED 7

static uint32_t timeoutCounter;
static bool active = true;

static const uint8_t brightness_delta = 1;
static uint8_t shelf_skip_counter = SHELF_EFFECT_SPEED;
static uint8_t shelf_counter_brightness = 0;

static uint8_t wall_skip_counter = WALL_EFFECT_SPEED;
static uint8_t current_wall_strip = 0;
static uint8_t current_wall_led = 0;

static uint8_t wave_middle = NUMLEDS_SHELF / 2;
static int8_t wave_direction = 1;
static uint8_t wave_skip_counter = WAVE_SPEED;

#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
microLED<NUMLEDS_SHELF, STRIP_SHELF_PIN, MLED_NO_CLOCK, LED_WS2811, ORDER_GRB, CLI_AVER> strip_shelf;
microLED<WALL_STRIPS * WALL_STRIP_LEDS, STRIP_WALL_PIN, MLED_NO_CLOCK, LED_WS2811, ORDER_GRB, CLI_AVER> strip_wall;

void brightness_increase_filler(uint8_t effect_speed, uint8_t& skip_counter, uint8_t& brightness_counter){
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

void wave(uint8_t effect_speed, uint8_t& skip_counter, uint8_t& middle, int8_t& wave_direction, uint8_t& brightness_counter) {
  if(skip_counter > 0) {
    skip_counter--;
    return;
  }
  skip_counter = effect_speed;
  if (middle == 3) {
    wave_direction = 1;
  }
  if (middle == NUMLEDS_SHELF - 4) {
    wave_direction = -1;
  }
  strip_shelf.set(middle-3, mWheel(SHELF_COLOR, brightness_counter));
  strip_shelf.set(middle-2, mWheel(SHELF_COLOR, brightness_counter + 10));
  strip_shelf.set(middle-1, mWheel(SHELF_COLOR, brightness_counter + 30));
  strip_shelf.set(middle, mWheel(SHELF_COLOR, brightness_counter + 60));
  strip_shelf.set(middle+1, mWheel(SHELF_COLOR, brightness_counter + 30));
  strip_shelf.set(middle+2, mWheel(SHELF_COLOR, brightness_counter + 10));
  strip_shelf.set(middle+3, mWheel(SHELF_COLOR, brightness_counter));
  middle += wave_direction;
}

void candle_filler(){
  if(active) {
    strip_wall.setBrightness(BRIGHTNESS);
    mData color = mPurple; // цвет свечи
    if(current_wall_led != 0 && ((current_wall_led+1) % WALL_STRIP_LEDS == 0)) {
      color = mWheel(random(0, 100), random(120, 230)); // цвет огня
    }
    strip_wall.set(current_wall_strip * WALL_STRIP_LEDS + current_wall_led, color);
    current_wall_led += 1;
    // последний лед в полоске?
    if(current_wall_led == WALL_STRIP_LEDS){
      current_wall_led = 0;
      current_wall_strip += 1;
    }
    // последняя полоска?
    if(current_wall_strip == WALL_STRIPS){
      current_wall_strip = 0;
    }
  } else {
    strip_wall.clear();
    current_wall_led = 0;
    current_wall_strip = 0;
  }
}

// заливка верхней полки
void shelf_filler() {
  brightness_increase_filler(SHELF_EFFECT_SPEED, shelf_skip_counter, shelf_counter_brightness);
  strip_shelf.setBrightness(shelf_counter_brightness);
  wave(WAVE_SPEED, wave_skip_counter, wave_middle, wave_direction, shelf_counter_brightness);
  strip_shelf.show();
}

// заливка стенки
void wall_filler() {
  if(wall_skip_counter > 0) {
    wall_skip_counter--;
    return;
  }
  wall_skip_counter = WALL_EFFECT_SPEED;
  candle_filler();
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
  //shelf_filler();
  wall_filler();
  delay(10);
}

void setup() {
  pinMode(PIR_SENSOR, INPUT);
  //strip_shelf.fill(mWheel(SHELF_COLOR, 0));
  strip_wall.setBrightness(0);
}

void loop() {
  loop_iteration();
}