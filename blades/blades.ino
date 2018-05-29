#include <Arduino.h>

#define APPDIR(X) APPDIR2(/home/hubbe/hack/teensy/lightsaber/lightsaber/X)
#define APPDIR2(X) #X

const unsigned int maxLedsPerStrip = 144;

int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

#include APPDIR(common/color.h)

#include "stm32l4_ws2811.h"
#include "pwm_pin.h"

WS2811Pin ws2811_pin;

SimplePWMPin<10> pwm_pin1;
SimplePWMPin<4> pwm_pin2;
SimplePWMPin<18> pwm_pin3;

// #define WS2811
#define PWM

void setup() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(9, OUTPUT);
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    Serial.println(i);
    delay(1000);
  }
  #ifdef WS2811
  ws2811_pin.begin(8, 1, WS2811Pin::Byteorder::BGR, 800000, 300);
  #endif

  #ifdef PWM
  Serial.println("INIT PWM");
  pwm_pin1.Activate();
  pwm_pin1.set(32768);
  delayMicroseconds(500);

  pwm_pin2.Activate();
  pwm_pin2.set(32768);

  pwm_pin3.Activate();
  pwm_pin1.set(32768);
  #else
  analogWrite(10, 127);
  #endif
  Serial.println("BEGIN");
}

uint64_t loop_count = 0;

void loop() {
  loop_count++;
  #ifdef WS2811
  digitalWrite(0, HIGH);
  Color8 c(loop_count & 0xff, 0xAA, 0x03);
  ws2811_pin.BeginFrame();
  digitalWrite(0, LOW);
  
  digitalWrite(1, HIGH);
  ws2811_pin.Set(0, c);
  ws2811_pin.EndFrame();
  digitalWrite(1, LOW);
  #endif

  #ifdef PWM
  uint32_t i = millis();
  pwm_pin1.set(i % 6000);
  pwm_pin2.set(i % 6000);
  pwm_pin3.set(i % 6000);
  #endif

  if (loop_count < 20)  {
     Serial.print("loop ");
     Serial.println(millis());
  }
}

