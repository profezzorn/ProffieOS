#ifndef BLADES_STM32L4_WS2811_H
#define BLADES_STM32L4_WS2811_H

#include <stm32l4_timer.h>
#include <stm32l4_dma.h>
#include <stm32l4_gpio.h>
#include <stm32l4_wiring_private.h>

namespace {
const int timer_frequency = 80000000;  // 80Mhz
};

class WS2811Client {
public:
  virtual void done_callback() = 0;
};

class WS2811Engine {
public:
  virtual void show(int pin, int bits, int frequency, WS2811Client* client) = 0;
  virtual bool done() = 0;
};

volatile bool ws2811_dma_done = true;

class WS2811EngineSTM32L4TIM2 : public WS2811Engine {
public:
  static const int instance = 1;
  
  static stm32l4_timer_t* timer() {
    return &stm32l4_pwm[instance];
  }

  WS2811EngineSTM32L4TIM2() {
    stm32l4_timer_create(timer(), TIMER_INSTANCE_TIM2, STM32L4_PWM_IRQ_PRIORITY, 0);
    stm32l4_dma_create(&dma_, DMA_CHANNEL_DMA1_CH2_TIM2_UP, STM32L4_PWM_IRQ_PRIORITY);

    // For proxy mode
    stm32l4_dma_create(&dma2_, DMA_CHANNEL_DMA1_CH1_TIM2_CH3, STM32L4_PWM_IRQ_PRIORITY);
    stm32l4_dma_create(&dma3_, DMA_CHANNEL_DMA1_CH5_TIM2_CH1, STM32L4_PWM_IRQ_PRIORITY);
  }
  ~WS2811EngineSTM32L4TIM2() {
    stm32l4_timer_destroy(timer());
    stm32l4_dma_destroy(&dma_);
    stm32l4_dma_destroy(&dma2_);
    stm32l4_dma_destroy(&dma3_);
  }

  void show(int pin, int bits, int frequency, WS2811Client* client) override {
    ((uint8_t*)displayMemory)[bits] = 0;
    int pulse_len = timer_frequency / frequency;
    int instance = g_APinDescription[pin].pwm_instance;
    int divider = stm32l4_timer_clock(timer()) / timer_frequency;
    ws2811_dma_done = false;
    pin_ = pin;
      
    if (g_PWMInstances[instance] != TIMER_INSTANCE_TIM2) {
      // Proxy mode, make sure GPIO A/B doesn't fall asleep
      RCC->AHB2SMENR |= (RCC_AHB2SMENR_GPIOASMEN | RCC_AHB2SMENR_GPIOBSMEN);
      
      stm32l4_dma_enable(&dma_, &dma_done_callback_ignore, (void*)client);
      stm32l4_dma_enable(&dma2_, NULL, NULL);
      stm32l4_dma_enable(&dma3_, &dma_done_callback, (void*)client);
      stm32l4_timer_enable(timer(),
			   divider -1,
			   pulse_len -1,
			   0 /* TIMER_OPTION_COUNT_PRELOAD */, NULL, NULL, 0);
      uint16_t bit = g_APinDescription[pin].bit;
      GPIO_TypeDef *GPIO = (GPIO_TypeDef *)(g_APinDescription[pin].GPIO);
      int offset = 0;
      if (bit > 128) {
	bit >>= 8;
	offset++;
      }
      bit_ = bit;

      uint8_t* data = (uint8_t*)displayMemory;
      
      // TODO: do the proper encoding upfront
      uint8_t t0h = 0;
      uint8_t t1h = 0;
      for (int i = 0; i < bits; i++) {
	uint8_t x = data[i];
	if (!x) continue;
	if (x < pulse_len/2) {
	  t0h = x;
	} else {
	  t1h = x;
	}
	if (t0h && t1h) break;
      }
      if (!t0h) t0h = pulse_len / 3;
      if (!t1h) t1h = pulse_len * 2 / 3;
      for (int i = bits; i > 0; i--) {
	data[i] = (data[i-1] == t0h) ? bit_ : 0;
      }

      #if 0
      STDOUT << data[0] << "," << data[1] << "," << data[2]<< "," << data[3]
	     << data[4] << "," << data[5] << "," << data[6]<< "," << data[7]<< "\n";
      // STDOUT << data[bits -1] << "," << data[bits] << "," << data[bits+1] << "\n";
      #endif
      
      stm32l4_timer_stop(timer());
      timer()->TIM->CNT = 0;
      
      armv7m_atomic_modify(&timer()->TIM->DIER, TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC3DE, 0);
      
      stm32l4_dma_start(&dma_, offset + (uint32_t)(&(GPIO->BSRR)),
			(uint32_t)&bit_,
			bits,
			DMA_OPTION_EVENT_TRANSFER_DONE |
			DMA_OPTION_MEMORY_TO_PERIPHERAL |
			DMA_OPTION_PERIPHERAL_DATA_SIZE_8 |
			DMA_OPTION_MEMORY_DATA_SIZE_8 |
			DMA_OPTION_PRIORITY_VERY_HIGH);
      stm32l4_dma_start(&dma2_, offset + (uint32_t)(&(GPIO->BRR)),
			(uint32_t)displayMemory,
			bits + 1,
			DMA_OPTION_EVENT_TRANSFER_DONE |
			DMA_OPTION_MEMORY_TO_PERIPHERAL |
			DMA_OPTION_PERIPHERAL_DATA_SIZE_8 |
			DMA_OPTION_MEMORY_DATA_SIZE_8 |
			DMA_OPTION_MEMORY_DATA_INCREMENT |
			DMA_OPTION_PRIORITY_VERY_HIGH);
      stm32l4_dma_start(&dma3_, offset + (uint32_t)(&(GPIO->BRR)),
			(uint32_t)&bit_,
			bits + 1,
			DMA_OPTION_EVENT_TRANSFER_DONE |
			DMA_OPTION_MEMORY_TO_PERIPHERAL |
			DMA_OPTION_PERIPHERAL_DATA_SIZE_8 |
			DMA_OPTION_MEMORY_DATA_SIZE_8 |
			DMA_OPTION_PRIORITY_VERY_HIGH);
      
      armv7m_atomic_or(&timer()->TIM->CR1, TIM_CR1_ARPE);
      
      stm32l4_timer_channel(timer(), TIMER_CHANNEL_1, t1h, TIMER_CONTROL_PWM);
      stm32l4_timer_channel(timer(), TIMER_CHANNEL_3, t0h, TIMER_CONTROL_PWM);
      stm32l4_gpio_pin_configure(g_APinDescription[pin].pin,
				 (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_OUTPUT));
      
      // Trigger DMA on update
      armv7m_atomic_or(&timer()->TIM->DIER, TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC3DE);
      stm32l4_timer_start(timer(), false);
      
    } else {
      stm32l4_dma_enable(&dma_, &dma_done_callback, (void*)client);
      
      volatile uint32_t* cmp_address = nullptr;
      switch (g_APinDescription[pin].pwm_channel) {
	case TIMER_CHANNEL_1: cmp_address = &timer()->TIM->CCR1; break;
	case TIMER_CHANNEL_2: cmp_address = &timer()->TIM->CCR2; break;
	case TIMER_CHANNEL_3: cmp_address = &timer()->TIM->CCR3; break;
	case TIMER_CHANNEL_4: cmp_address = &timer()->TIM->CCR4; break;
	case TIMER_CHANNEL_5: cmp_address = &timer()->TIM->CCR5; break;
	case TIMER_CHANNEL_6: cmp_address = &timer()->TIM->CCR6; break;
      }
      
      stm32l4_timer_enable(timer(),
			   divider -1,
			   pulse_len -1,
			   0 /* TIMER_OPTION_COUNT_PRELOAD */, NULL, NULL, 0);
      stm32l4_timer_stop(timer());
      timer()->TIM->CNT = 0;
      
      armv7m_atomic_modify(&timer()->TIM->DIER, TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC3DE, 0);
      
      stm32l4_dma_start(&dma_, (uint32_t)cmp_address,
			(uint32_t)displayMemory,
			bits + 1,
			DMA_OPTION_EVENT_TRANSFER_DONE |
			DMA_OPTION_MEMORY_TO_PERIPHERAL |
			DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
			DMA_OPTION_MEMORY_DATA_SIZE_8 |
			DMA_OPTION_MEMORY_DATA_INCREMENT |
			DMA_OPTION_PRIORITY_HIGH);
      
      
      armv7m_atomic_or(&timer()->TIM->CR1, TIM_CR1_ARPE);
      
      stm32l4_timer_start(timer(), false);
      stm32l4_timer_channel(timer(), g_APinDescription[pin].pwm_channel, 0, TIMER_CONTROL_PWM);
      stm32l4_gpio_pin_configure(g_APinDescription[pin].pin,
				 (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
      
      // Trigger DMA on update
      armv7m_atomic_or(&timer()->TIM->DIER, TIM_DIER_UDE);
    }
  }
    
  bool done() override {
    return ws2811_dma_done;
  }

  static void dma_done_callback_ignore(void* context, uint32_t events) {
  }
  static void dma_done_callback(void* context, uint32_t events) {
    // Set the pin to low, normal output mode. This will keep the pin low even if we
    // re-use the timer for another show() call.
    digitalWrite(pin_, LOW);
    stm32l4_gpio_pin_configure(g_APinDescription[pin_].pin,
			       (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_OUTPUT));
    stm32l4_timer_stop(timer());
    stm32l4_timer_disable(timer());
    stm32l4_dma_stop(&dma_);
    stm32l4_dma_stop(&dma2_);
    stm32l4_dma_stop(&dma3_);
    stm32l4_dma_disable(&dma_);
    stm32l4_dma_disable(&dma2_);
    stm32l4_dma_disable(&dma3_);
    ws2811_dma_done = true;
    // GPIO A/B may sleep on WFE now.
    RCC->AHB2SMENR &= ~(RCC_AHB2SMENR_GPIOASMEN | RCC_AHB2SMENR_GPIOBSMEN);
    ((WS2811Client*)context)->done_callback();
  }

private:
  static volatile int pin_;
  static volatile uint8_t bit_;
  static stm32l4_timer_t timer_;
  static stm32l4_dma_t dma_;
  static stm32l4_dma_t dma2_;
  static stm32l4_dma_t dma3_;
};

volatile int WS2811EngineSTM32L4TIM2::pin_;
stm32l4_timer_t WS2811EngineSTM32L4TIM2::timer_;
stm32l4_dma_t WS2811EngineSTM32L4TIM2::dma_;
stm32l4_dma_t WS2811EngineSTM32L4TIM2::dma2_;
stm32l4_dma_t WS2811EngineSTM32L4TIM2::dma3_;
volatile uint8_t WS2811EngineSTM32L4TIM2::bit_;


WS2811Engine* GetWS2811Engine(int pin) {
  static WS2811EngineSTM32L4TIM2 engine;
  if (pin < 0) return nullptr;
  return &engine;
}

class WS2811Pin : public WS2811Client {
public:
  using Byteorder = Color8::Byteorder;

  // Note, t0h_us/t1h_us are out of 1250 us, even if the frequency is
  // not 800kHz. If you're reading the datasheet for a 400Hz strip, you
  // would need to divide by two.
  WS2811Pin(int pin,
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
    while (Color8::num_bytes(byteorder_) * num_leds_ * 8 + 1 > (int)sizeof(displayMemory)) {
      STDOUT.print("Display memory is not big enough, increase maxLedsPerStrip!");
      num_leds_ /= 2;
    }
    while (!IsReadyForBeginFrame());
  }

  void Set(int led, Color8 color) {
    uint32_t *output = ((uint32_t *)displayMemory) + led * Color8::num_bytes(byteorder_) * 2;
    for (int i = Color8::num_bytes(byteorder_) - 1; i >= 0; i--) {
      uint32_t tmp = color.getByte(byteorder_, i) * 0x8040201U;
      *(output++) = zero4X_ + ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
      *(output++) = zero4X_ + ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
    }
  }

  void Set(int led, Color16 color) { Set(led, color.dither(frame_num_, led)); }
  // void Set(int led, Color16 color) { Set(led, color.dither(0)); }

  bool IsReadyForEndFrame() {
    return done_ && (micros() - done_time_us_) > reset_us_;
  }

  void EndFrame() {
    if (!engine_) return;
    while (!IsReadyForEndFrame());
    done_ = false;
    frame_num_++;
    if (engine_)
      engine_->show(pin_, num_leds_ * Color8::num_bytes(byteorder_) * 8, frequency_, this);
  }

  int num_leds() const { return num_leds_; }
  Color8::Byteorder get_byteorder() const { return byteorder_; }
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
  uint32_t reset_us_;

  uint32_t one_minus_zero_;
  uint32_t zero4X_;

  volatile bool done_ = true;
  volatile uint32_t done_time_us_ = 0;
};


#endif
