#ifndef BLADES_STM32L4_WS2811_H
#define BLADES_STM32L4_WS2811_H

#include <stm32l4_timer.h>
#include <stm32l4_dma.h>
#include <stm32l4_gpio.h>
#include <stm32l4_wiring_private.h>

// common
int displayMemory[maxLedsPerStrip * 24 / 4 + 1];

namespace {

const int timer_frequency = 80000000;  // 80Mhz
  
static int8_t dither_matrix[4][4] = {
  { -127, 111,  -76,  94 },
  {    9, -59,   60,  -8 },
  {  -93,  77, -110, 127 },
  {   43, -25,   26, -42 },
};

};

class WS2811Client {
public:
  virtual void done_callback() = 0;
};

class WS2811Engine {
public:
  virtual void show(int pin, int num_leds, int frequency, WS2811Client* client) = 0;
  virtual bool done() = 0;
};

class WS2811EngineSTM32L4TIM2 : public WS2811Engine {
public:
  WS2811EngineSTM32L4TIM2() {
    stm32l4_timer_create(&timer_, TIMER_INSTANCE_TIM2, STM32L4_PWM_IRQ_PRIORITY, 0);
    stm32l4_dma_create(&dma_, DMA_CHANNEL_DMA1_CH2_TIM2_UP, STM32L4_PWM_IRQ_PRIORITY);
  }
  ~WS2811EngineSTM32L4TIM2() {
    stm32l4_timer_destroy(&timer_);
    stm32l4_dma_destroy(&dma_);
  }

  void show(int pin, int num_leds, int frequency, WS2811Client* client) override {
    displayMemory[num_leds * 24/4] = 0;
    int pulse_len = timer_frequency / frequency;
    int instance = g_APinDescription[pin].pwm_instance;
    if (g_PWMInstances[instance] != TIMER_INSTANCE_TIM2) {
      Serial.println("Wrong WS2811 engine or pin!");
      return;
    }

    int divider = stm32l4_timer_clock(&timer_) / timer_frequency;

    dma_done_ = false;
    pin_ = pin;
    
    stm32l4_dma_enable(&dma_, &dma_done_callback, (void*)client);

    volatile uint32_t* cmp_address = nullptr;
    switch (g_APinDescription[pin].pwm_channel) {
    case TIMER_CHANNEL_1: cmp_address = &timer_.TIM->CCR1; break;
    case TIMER_CHANNEL_2: cmp_address = &timer_.TIM->CCR2; break;
    case TIMER_CHANNEL_3: cmp_address = &timer_.TIM->CCR3; break;
    case TIMER_CHANNEL_4: cmp_address = &timer_.TIM->CCR4; break;
    case TIMER_CHANNEL_5: cmp_address = &timer_.TIM->CCR5; break;
    case TIMER_CHANNEL_6: cmp_address = &timer_.TIM->CCR6; break;
    }

    stm32l4_timer_enable(&timer_,
			 divider -1,
			 pulse_len -1,
			 0 /* TIMER_OPTION_COUNT_PRELOAD */, NULL, NULL, 0);
    stm32l4_timer_stop(&timer_);
    timer_.TIM->CNT = 0;

    armv7m_atomic_modify(&timer_.TIM->DIER, TIM_DIER_UDE, 0);

    stm32l4_dma_start(&dma_, (uint32_t)cmp_address,
		      (uint32_t)displayMemory,
		      num_leds * 24 + 1,
                      DMA_OPTION_EVENT_TRANSFER_DONE |
                      DMA_OPTION_MEMORY_TO_PERIPHERAL |
                      DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
                      DMA_OPTION_MEMORY_DATA_SIZE_8 |
                      DMA_OPTION_MEMORY_DATA_INCREMENT |
                      DMA_OPTION_PRIORITY_HIGH);


    armv7m_atomic_or(&timer_.TIM->CR1, TIM_CR1_ARPE);

    stm32l4_timer_start(&timer_, false);
    stm32l4_timer_channel(&timer_, g_APinDescription[pin].pwm_channel, 0, TIMER_CONTROL_PWM);
    stm32l4_gpio_pin_configure(g_APinDescription[pin].pin,
			       (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));

    // Trigger DMA on update
    armv7m_atomic_or(&timer_.TIM->DIER, TIM_DIER_UDE);
  }

  bool done() override {
    return dma_done_;
  }

  static void dma_done_callback(void* context, uint32_t events) {
    // Set the pin to low, normal output mode. This will keep the pin low even if we
    // re-use the timer for another show() call.
    digitalWrite(pin_, LOW);
    stm32l4_gpio_pin_configure(g_APinDescription[pin_].pin,
			       (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_OUTPUT));
    dma_done_ = true;
    ((WS2811Client*)context)->done_callback();
  }

private:
  static volatile bool dma_done_;
  static volatile int pin_;
  static stm32l4_timer_t timer_;
  static stm32l4_dma_t dma_;
};

volatile bool WS2811EngineSTM32L4TIM2::dma_done_ = true;
volatile int WS2811EngineSTM32L4TIM2::pin_;
stm32l4_timer_t WS2811EngineSTM32L4TIM2::timer_;
stm32l4_dma_t WS2811EngineSTM32L4TIM2::dma_;

WS2811Engine* GetWS2811Engine(int pin) {
  static WS2811EngineSTM32L4TIM2 engine;
  if (pin < 0 || g_APinDescription[pin].pwm_instance != 1)
    return nullptr;
  return &engine;
}

class WS2811Pin : public WS2811Client {
public:
  enum Byteorder {
    BGR=0x321,
    BRG=0x312,
    GBR=0x231,
    GRB=0x213,
    RBG=0x132,
    RGB=0x123,
  };

  // Note, t0h_us/t1h_us are out of 1250 us, even if the frequency is
  // not 800kHz. If you're reading the datasheet for a 400Hz strip, you
  // would need to divide by two.
  void begin(int pin,
	     int num_leds,
             Byteorder byteorder,
	     int frequency,
	     int reset_us,
	     int t0h_us = 294,
	     int t1h_us = 862) {
    engine_ = GetWS2811Engine(pin);
    pin_ = pin;
    num_leds_ = num_leds;
    byteorder_ = byteorder;
    frequency_ = frequency;
    reset_us_ = reset_us;
    int pulse_len = timer_frequency / frequency;
    // Make these tunable
    int zero_len = pulse_len * t0h_us / 1250;
    int one_len = pulse_len * t1h_us / 1250;
    zero4X_ = zero_len * 0x01010101U;
    one_minus_zero_ = one_len - zero_len;
#if 0    
    if (!engine_) {
      Serial.print("Pin ");
      Serial.print(pin);
      Serial.println(" cannot do WS2811 output.");
    }
#endif    
  }

  bool IsReadyForBeginFrame() {
    if (!engine_) return true;
    return engine_->done();
  }

  void BeginFrame() {
    while (!IsReadyForBeginFrame());
  }

  void Set(int led, Color8 color) {
    uint32_t *output = ((uint32_t *)displayMemory) + led * (24/4);
    uint8_t rgb[3];
    rgb[0] = color.r;
    rgb[1] = color.g;
    rgb[2] = color.b;
    for (int i = 2; i >= 0; i--) {
      uint32_t tmp = rgb[(( ((int)byteorder_) >> (i * 4)) & 0xf) - 1] * 0x8040201U;
      *(output++) = zero4X_ + ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
      *(output++) = zero4X_ + ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
    }
  }

  void Set(int led, Color16 color) {
    uint32_t *output = ((uint32_t *)displayMemory) + led * (24/4);
    uint8_t rgb[3];
    int dither = dither_matrix[frame_num_ & 3][led & 3];
    rgb[0] = clampi32((color.r + dither) >> 8, 0, 255);
    rgb[1] = clampi32((color.g + dither) >> 8, 0, 255);
    rgb[2] = clampi32((color.b + dither) >> 8, 0, 255);
    for (int i = 2; i >= 0; i--) {
      uint32_t tmp = rgb[(( ((int)byteorder_) >> (i * 4)) & 0xf) - 1] * 0x8040201U;
      *(output++) = zero4X_ + ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
      *(output++) = zero4X_ + ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
    }
  }

  bool IsReadyForEndFrame() {
    return done_ && (micros() - done_time_us_) > reset_us_;
  }

  void EndFrame() {
    if (!engine_) return;
    while (!IsReadyForEndFrame());
    done_ = false;
    frame_num_++;
    if (engine_)
      engine_->show(pin_, num_leds_, frequency_, this);
  }

  int num_leds() const { return num_leds_; }
  int pin() { return pin_; }

private:
  void done_callback() override {
    done_time_us_ = micros();
    done_ = true;
  }

  WS2811Engine* engine_;
  uint8_t frame_num_ = 0;
  int8_t pin_;
  int16_t num_leds_;
  Byteorder byteorder_;
  int frequency_;
  int reset_us_;

  uint32_t one_minus_zero_;
  uint32_t zero4X_;

  volatile bool done_ = true;
  volatile uint32_t done_time_us_ = 0;
};


#endif
