#include <Arduino.h>
// простейшие динамические эффекты
// сначала ознакомься с примером microLED_guide !!!

#define STRIP1_PIN 9     // пин полки
#define STRIP2_PIN 8    // пин стенки
#define NUMLEDS1 20
#define NUMLEDS2 4
#define TIMEOUT 5
#define PIR_SENSOR 12

static uint32_t timeoutCounter;
static int counter1 = 0;
static byte running_dots_counter = 0;
static bool active = false;

#define COLOR_DEBTH 3
#include <microLED.h>   // подключаем библу
microLED<NUMLEDS1, STRIP1_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip1;
microLED<NUMLEDS2, STRIP2_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip2;

void filler1() {
  strip1.clear(); // clear
  if(counter1 > 0) strip1.fill(0, counter1, mRGB(200, 160, 0)); // fill strip to led counter
  strip1.show();
}

void filler2() {
  strip2.clear(); // clear
  if(active) { // fill strip to led counter
    if (running_dots_counter == NUMLEDS2) {
      strip2.leds[0] = mRed;
      running_dots_counter = 0;
    }
    strip2.leds[running_dots_counter] = mWhite;
    running_dots_counter++;
  }
  strip2.show();
}

void update_counter() {
  if(active) {  // if motion scanner is activated
    if (counter1 < (NUMLEDS1 - 1)) counter1++; // increase amount of active leds
  } else {
    if(counter1 > 0) counter1--;  // decrease active leds
  }
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
  update_counter();
  filler1();
  filler2();
}

void setup() {
  pinMode(PIR_SENSOR, INPUT);
  strip1.setBrightness(150);
  strip2.setBrightness(100);
}

void loop() {
  loop_iteration();
  delay(100);
}