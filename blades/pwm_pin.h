#ifndef BLADES_PWM_PIN_H
#define BLADES_PWM_PIN_H

#include "led_interface.h"

#ifdef ARDUINO_ARCH_STM32L4
#include <stm32l4_timer.h>

namespace {
static stm32l4_timer_t stm32l4_pwm[PWM_INSTANCE_COUNT];
static uint8_t timer_use_counts[PWM_INSTANCE_COUNT];

#define PWM_SYNC_INSTANCE 3  // TIM15


void SetupTimer(uint32_t instance) {
  timer_use_counts[instance]++;
  if (stm32l4_pwm[instance].state == TIMER_STATE_NONE) {
    stm32l4_timer_create(&stm32l4_pwm[instance], g_PWMInstances[instance], 15, 0);
  }

  if (stm32l4_pwm[instance].state == TIMER_STATE_INIT) {
    // 813 Hz, 32768 steps
    uint32_t carrier = 26666666;
    uint32_t modulus = 32768;
    uint32_t divider = stm32l4_timer_clock(&stm32l4_pwm[instance]) / carrier;
    
    if (divider == 0) divider = 1;
    
    stm32l4_timer_enable(&stm32l4_pwm[instance], divider -1, modulus -1, 0, NULL, NULL, 0);
    stm32l4_timer_start(&stm32l4_pwm[instance], false);

    if (instance != PWM_SYNC_INSTANCE)  {
      SetupTimer(PWM_SYNC_INSTANCE);
      // TIM16 cannot be synchronized in hardware, so let's do the best we can.
      // We use TIM15 to synchromize with, because it is used for PWM on all proffieboards.
      volatile uint32_t* from = &stm32l4_pwm[PWM_SYNC_INSTANCE].TIM->CNT;
      volatile uint32_t* to = &stm32l4_pwm[instance].TIM->CNT;
      noInterrupts();
      *to = *from + 10;
      interrupts();
    } else {
      // avoid slow startup
      stm32l4_pwm[instance].TIM->CNT = 0;
    }
    // Buffer counters from now on.
    stm32l4_pwm[instance].TIM->CR1 |= TIM_CR1_ARPE;
  }
}

void TeardownTimer(uint32_t instance) {
  if (0 == --timer_use_counts[instance]) {
    stm32l4_timer_stop(&stm32l4_pwm[instance]);
    stm32l4_timer_disable(&stm32l4_pwm[instance]);
    if (instance != PWM_SYNC_INSTANCE) {
      TeardownTimer(PWM_SYNC_INSTANCE);
    }
  }
}

void LSanalogWrite(uint32_t pin, int value) {
  TIM_TypeDef* TIM = stm32l4_pwm[g_APinDescription[pin].pwm_instance].TIM;
  value >>= 1;
  if (value < 0) value = 0;
  if (value > 32768) value = 32768;
  // stm32l4_timer_channel(&stm32l4_pwm[instance], g_APinDescription[pin].pwm_channel, value, TIMER_CONTROL_PWM);
  switch (g_APinDescription[pin].pwm_channel) {
    case TIMER_CHANNEL_1: TIM->CCR1 = value; break;
    case TIMER_CHANNEL_2: TIM->CCR2 = value; break;
    case TIMER_CHANNEL_3: TIM->CCR3 = value; break;
    case TIMER_CHANNEL_4: TIM->CCR4 = value; break;
    case TIMER_CHANNEL_5: TIM->CCR5 = value; break;
    case TIMER_CHANNEL_6: TIM->CCR6 = value; break;
  }
}

void LSanalogWriteSetup(uint32_t pin) {
  // Handle the case the pin isn't usable as PIO
  if (pin >= NUM_TOTAL_PINS || g_APinDescription[pin].GPIO == NULL) {
    Serial.print("Analog Setup: NOT A PIN: ");
    Serial.println(pin);
    return;
  }
  
  if (!(g_APinDescription[pin].attr & PIN_ATTR_PWM)) {
    Serial.println("Analog Setup: Pin is not configured for PWM: ");
    Serial.println(pin);
    return;
  }
  uint32_t instance = g_APinDescription[pin].pwm_instance;
  SetupTimer(instance);
  stm32l4_timer_channel(&stm32l4_pwm[instance], g_APinDescription[pin].pwm_channel, 0, TIMER_CONTROL_PWM);
  // Wait for a complete cycle to make sure the internal state is clear before setting the output mode.
  delayMicroseconds(1300); // 1.3ms
  stm32l4_gpio_pin_configure(g_APinDescription[pin].pin, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
}

void LSanalogWriteTeardown(uint32_t pin) {
  pinMode(pin, INPUT_ANALOG);
  TeardownTimer(g_APinDescription[pin].pwm_instance);
}


};
#elif defined(ESP32)
// First some abstractions for controlling PWM pin

#ifdef SOC_LEDC_SUPPORT_HS_MODE
#define LEDC_CHANNELS           (SOC_LEDC_CHANNEL_NUM<<1)
#else
#define LEDC_CHANNELS           (SOC_LEDC_CHANNEL_NUM)
#endif

static int8_t po_pin_to_channel[SOC_GPIO_PIN_COUNT] = { 0 };
static int po_cnt_channel = LEDC_CHANNELS;

void LSanalogWriteSetup(uint32_t pin) {
  if (po_pin_to_channel[pin] == 0) {
    if (!po_cnt_channel) {
      log_e("No more analogWrite channels available! You can have maximum %u", LEDC_CHANNELS);
      return;
    }
    po_pin_to_channel[pin] = po_cnt_channel--;
    ledcAttachPin(pin, po_cnt_channel);
    ledcSetup(po_cnt_channel, 1000, 16);
  }
}
void LSanalogWriteTeardown(uint32_t pin) {
  ledcDetachPin(pin);
}
void LSanalogWrite(uint32_t pin, int value) {
  ledcWrite(po_pin_to_channel[pin] - 1, value);
}
#elif defined(TEENSYDUINO)
// First some abstractions for controlling PWM pin
void LSanalogWriteSetup(uint32_t pin) {
  analogWriteResolution(16);
  analogWriteFrequency(pin, 1000);
}
void LSanalogWriteTeardown(uint32_t pin) {}
void LSanalogWrite(uint32_t pin, int value) {
  analogWrite(pin, value);
}
#else
// First some abstractions for controlling PWM pin
void LSanalogWriteSetup(uint32_t pin) {
  analogWriteResolution(16);
}
void LSanalogWriteTeardown(uint32_t pin) {}
void LSanalogWrite(uint32_t pin, int value) {
  analogWrite(pin, value);
}
#endif

class PWMPinInterface {
public:
  virtual void Activate() = 0;
  virtual void Deactivate() = 0;
  virtual void set(const Color16& c) = 0;
  virtual void set_overdrive(const Color16& c) = 0;
  virtual bool rgb() { return false; }
};

template<int PIN>
class SimplePWMPin {
public:
  void Activate() {
    static_assert(PIN >= -1, "PIN is negative");
    LSanalogWriteSetup(PIN);
    LSanalogWrite(PIN, 0);  // make it black
  }
  void Deactivate() {
    LSanalogWrite(PIN, 0);  // make it black
    LSanalogWriteTeardown(PIN);
  }
  void set(int32_t v) {
    LSanalogWrite(PIN, v);
  }
};

#endif
