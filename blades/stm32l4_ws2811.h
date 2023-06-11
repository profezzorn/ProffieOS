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
  virtual int chunk_size() = 0;
  virtual int pin() const = 0;
  virtual int frequency() = 0;
  virtual int num_leds() = 0;
  virtual void read(uint8_t* dest) = 0;
  virtual uint8_t get_t0h() = 0;
  virtual uint8_t get_t1h() = 0;
  virtual void set01(uint8_t zero, uint8_t one) = 0;
  WS2811Client* volatile next_ws2811_client_ = nullptr;
};

#ifdef DEBUG
#define PROFFIEOS_ASSERT(X) do {					\
  if (!(X)) {								\
    interrupts();							\
    if (!(X)) STDERR << "ASSERT " << #X << " FAILED @ " << __FILE__ << ":" << __LINE__ << "\n"; \
    while(true);							\
  }									\
} while(0)
#else
#define PROFFIEOS_ASSERT(X) do {} while(0)
#endif

// Position where we are reading colors from the color buffer.
Color16* volatile color_buffer_ptr = color_buffer;
uint32_t color_buffer_size = 0;

class WS2811Engine {
public:
  virtual void queue(WS2811Client* client) = 0;
  virtual void kick() = 0;
};

volatile bool ws2811_dma_done = true;

class NoInterruptScope {
public:
  NoInterruptScope() { noInterrupts(); }
  ~NoInterruptScope() { interrupts(); }
};


class WS2811EngineSTM32L4 : public WS2811Engine {
public:
#if PROFFIEBOARD_VERSION == 3
  static const int instance = 0;
#define WS2811_TIMER_INSTANCE TIMER_INSTANCE_TIM1
#else  
#define WS2811_TIMER_INSTANCE TIMER_INSTANCE_TIM2
  static const int instance = 1;
#endif  
  
  static stm32l4_timer_t* timer() {
    return &stm32l4_pwm[instance];
  }

  void kick() override {
    TRACE(BLADE, "kick");
    {
      NoInterruptScope NI;
      if (!ws2811_dma_done) return;
      if (!client_) return;
      ws2811_dma_done = false;
    }
    PROFFIEOS_ASSERT(color_buffer_size);
    show();
  }

  void queue(WS2811Client* client) override {
    TRACE(BLADE, "queue");
    client->next_ws2811_client_ = nullptr;
    PROFFIEOS_ASSERT(color_buffer_size);
    {
      NoInterruptScope NI; 
      if (!client_) {
	last_client_ = client_ = client;
      } else {
	PROFFIEOS_ASSERT(client_ != client);
	PROFFIEOS_ASSERT(last_client_ != client);
	last_client_->next_ws2811_client_ = client;
	last_client_ = client;
      }
    }
    kick();
  }

  // Copy of stm32l4_timer_channel, but without the buffering.
  bool timer_channel(unsigned int channel, uint32_t compare) {
    TIM_TypeDef *TIM = timer()->TIM;
    uint32_t tim_ccmr, tim_ccer;
    
    if ((timer()->state != TIMER_STATE_READY) && (timer()->state != TIMER_STATE_ACTIVE)) {
      return false;
    }
    
    armv7m_atomic_and(&TIM->CCER, ~(TIM_CCER_CC1E << (channel * 4)));
    
    tim_ccmr = 0;
    tim_ccer = 0;

    // TIMER_CONTROL_PWM
    tim_ccmr |= 6 << 4;
    tim_ccer |= (TIM_CCER_CC1E | TIM_CCER_CC1NE | TIM_CCER_CC1NP);
    
    if (channel <= TIMER_CHANNEL_4) {
      armv7m_atomic_or(&timer()->channels, (TIMER_EVENT_CHANNEL_1 << channel));
    }
    
    if (channel <= TIMER_CHANNEL_4) {
      if ((timer()->channels & timer()->events) & (TIMER_EVENT_CHANNEL_1 << channel)) {
	armv7m_atomic_or(&TIM->DIER, (TIM_DIER_CC1IE << channel));
      } else {
	armv7m_atomic_and(&TIM->DIER, ~(TIM_DIER_CC1IE << channel));
      }
    }
    
    switch (channel) {
      case TIMER_CHANNEL_1: armv7m_atomic_modify(&TIM->CCMR1, 0x000100ff, (tim_ccmr << 0)); TIM->CCR1 = compare; break;
      case TIMER_CHANNEL_2: armv7m_atomic_modify(&TIM->CCMR1, 0x0100ff00, (tim_ccmr << 8)); TIM->CCR2 = compare; break;
      case TIMER_CHANNEL_3: armv7m_atomic_modify(&TIM->CCMR2, 0x000100ff, (tim_ccmr << 0)); TIM->CCR3 = compare; break;
      case TIMER_CHANNEL_4: armv7m_atomic_modify(&TIM->CCMR2, 0x0100ff00, (tim_ccmr << 8)); TIM->CCR4 = compare; break;
      case TIMER_CHANNEL_5: armv7m_atomic_modify(&TIM->CCMR2, 0x000100ff, (tim_ccmr << 0)); TIM->CCR5 = compare; break;
      case TIMER_CHANNEL_6: armv7m_atomic_modify(&TIM->CCMR2, 0x0100ff00, (tim_ccmr << 8)); TIM->CCR6 = compare; break;
    }
    
    armv7m_atomic_modify(&TIM->CCER, (0x0000000f << (channel * 4)), (tim_ccer << (channel * 4)));
    
    return true;
  }
  
  WS2811EngineSTM32L4() {
    // Priority is 1 step above audio, because this code freezes if we
    // miss an interrupt. If we miss an audio interrupt, we just get a small glitch.
    int irq_priority = STM32L4_SAI_IRQ_PRIORITY - 1;
#if PROFFIEBOARD_VERSION == 3
    stm32l4_timer_create(timer(), TIMER_INSTANCE_TIM1, irq_priority, 0);
    stm32l4_dma_create(&dma_, DMA_CHANNEL_DMA1_CH6_TIM1_UP, irq_priority);
    
    // For proxy mode
    stm32l4_dma_create(&dma2_, DMA_CHANNEL_DMA1_CH7_TIM1_CH3, irq_priority);
    stm32l4_dma_create(&dma3_, DMA_CHANNEL_DMA1_CH2_TIM1_CH1, irq_priority);
#else
    stm32l4_timer_create(timer(), TIMER_INSTANCE_TIM2, irq_priority, 0);
    stm32l4_dma_create(&dma_, DMA_CHANNEL_DMA1_CH2_TIM2_UP, irq_priority);
    
    // For proxy mode
    stm32l4_dma_create(&dma2_, DMA_CHANNEL_DMA1_CH1_TIM2_CH3, irq_priority);
    stm32l4_dma_create(&dma3_, DMA_CHANNEL_DMA1_CH5_TIM2_CH1, irq_priority);
#endif    
  }
  ~WS2811EngineSTM32L4() {
    stm32l4_timer_destroy(timer());
    stm32l4_dma_destroy(&dma_);
    stm32l4_dma_destroy(&dma2_);
    stm32l4_dma_destroy(&dma3_);
  }

private:
  static void flush_dma(stm32l4_dma_t *dma) {
    stm32l4_dma_enable(dma, &dma_done_callback_ignore, (void*)NULL);
    uint32_t foo;
    stm32l4_dma_start(dma, (uint32_t)&foo,
		      (uint32_t) &(RCC->AHB2SMENR),  // any register should work
		      1,
		      DMA_OPTION_EVENT_TRANSFER_DONE |
		      DMA_OPTION_PERIPHERAL_TO_MEMORY |
		      DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
		      DMA_OPTION_MEMORY_DATA_SIZE_32 |
		      DMA_OPTION_PRIORITY_VERY_HIGH);
    stm32l4_dma_stop(dma);
    stm32l4_dma_disable(dma);
  }

  void DoRead(uint8_t* dest) {
    if (read_calls_) {
      read_calls_--;
      client_->read(dest);
    } else {
      memset(dest, 0, chunk_size_);
    }
  }

  void FillTo(uint8_t* end) {
#if 0    
    while (dest_ < end) {
      if (!chunk_bits_) {
	DoRead((uint8_t*)displayMemory);
	chunk_bits_ = chunk_size_;
      }
      int to_copy = std::min<int>(end - dest_, chunk_bits_);
      memcpy(dest_, displayMemory + chunk_size_ - chunk_bits_, to_copy);
      dest_ += to_copy;
      chunk_bits_ -= to_copy;
    }
#else
    // Faster, but assumes unaligned memory access is ok.
    while (dest_ < end) {
      if (!chunk_bits_) {
	while (dest_ <= end - chunk_size_) {
	  DoRead(dest_);
	  dest_ += chunk_size_;
	}
	DoRead((uint8_t*)displayMemory);
	chunk_bits_ = chunk_size_;
      }
      int to_copy = std::min<int>(end - dest_, chunk_bits_);
      memcpy(dest_, displayMemory + chunk_size_ - chunk_bits_, to_copy);
      dest_ += to_copy;
      chunk_bits_ -= to_copy;
    }
#endif    
  }

  void Fill(bool top) {
    if (dest_ == end_) dest_ = begin_;
    if (top) {
      FillTo(half_);
    } else {
      FillTo(end_);
    }
  }
  
  void show()  {
    WS2811Client* client = client_;
    PROFFIEOS_ASSERT(client);
    int pin = client->pin();
    int leds = client->num_leds();
    int frequency = client->frequency();
    TRACE(BLADE, "show enter");

    chunk_size_ = client->chunk_size();
    int total_chunks = (sizeof(displayMemory) - 1) / chunk_size_ - 1;
    int bits = leds * chunk_size_;
    int chunks_per_interrupt = total_chunks / 2;
    bool CIRCULAR = total_chunks < leds;
    read_calls_ = leds;
    if (chunks_per_interrupt > 32 && CIRCULAR) chunks_per_interrupt = 32;
    bits_per_interrupt_ = chunks_per_interrupt * chunk_size_;

    // Divvy up the displayMemory into three regions:
    // 1: A chunk for reading data into (one chunk)  (displayMemory)
    // 2: upper dma buffer                           (begin_)
    // 3: lower dma buffer                           (half_)
    //                                               (end_)
    
    begin_ = (uint8_t*)displayMemory;
    begin_ += chunk_size_;
    dest_ = begin_;
    half_ = begin_ + chunks_per_interrupt * chunk_size_;
    end_ = half_ + chunks_per_interrupt * chunk_size_;
    chunk_bits_ = 0;
    sent_ = 0;

    int pulse_len = timer_frequency / frequency;
    int instance = g_APinDescription[pin].pwm_instance;
    int divider = stm32l4_timer_clock(timer()) / timer_frequency;
    pin_ = pin;
    
    if (instance == PWM_INSTANCE_NONE || g_PWMInstances[instance] != WS2811_TIMER_INSTANCE) {
      TRACE(BLADE, "proxy");
      // Proxy mode, make sure GPIO A/B/C doesn't fall asleep
      RCC->AHB2SMENR |= (RCC_AHB2SMENR_GPIOASMEN | RCC_AHB2SMENR_GPIOBSMEN | RCC_AHB2SMENR_GPIOCSMEN);
      stm32l4_timer_enable(timer(),
			   divider -1,
			   pulse_len -1,
			   0 /* TIMER_OPTION_COUNT_PRELOAD */, NULL, NULL, 0);
      // Enabling dma sometimes triggers an interrupt, this helps us ignore it.
      stm32l4_dma_enable(&dma2_, &dma_done_callback_ignore, (void*)NULL);
      stm32l4_dma_enable(&dma3_, &dma_done_callback_ignore, (void*)NULL);
      
      stm32l4_dma_enable(&dma_, &dma_done_callback_ignore, (void*)this);
      if (CIRCULAR) {
	stm32l4_dma_enable(&dma2_, &dma_refill_callback2, (void*)this);
      } else {
	stm32l4_dma_enable(&dma2_, &dma_done_callback_ignore, nullptr);
      }
      stm32l4_dma_enable(&dma3_, &dma_done_callback, (void*)this);
      uint16_t bit = g_APinDescription[pin].bit;
      GPIO_TypeDef *GPIO = (GPIO_TypeDef *)(g_APinDescription[pin].GPIO);
      int offset = 0;
      if (bit > 128) {
	bit >>= 8;
	offset++;
      }
      bit_ = bit;

      int t0h = client->get_t0h();
      int t1h = client->get_t1h();
      client->set01(bit_, 0);

      // Need to insert a zero at the beginning.
      *(dest_++) = 0;

      Fill(false);
      
#if 0
      STDOUT << data[0] << "," << data[1] << "," << data[2]<< "," << data[3] << ","
	     << data[4] << "," << data[5] << "," << data[6]<< "," << data[7]<< "\n";
      // STDOUT << data[bits -1] << "," << data[bits] << "," << data[bits+1] << "\n";
#endif
      
      TRACE(BLADE, "stop!");
      stm32l4_timer_stop(timer());
      TRACE(BLADE, "cnt=0");
      timer()->TIM->CNT = 0;
      
      // armv7m_atomic_or(&timer()->TIM->CR1, TIM_CR1_ARPE);
      
      TRACE(BLADE, "ch1");
      timer_channel(TIMER_CHANNEL_1, t1h);
      TRACE(BLADE, "ch3");
      timer_channel(TIMER_CHANNEL_3, t0h);
      
      TRACE(BLADE, "SR");
      timer()->TIM->SR = 0;
      
      TRACE(BLADE, "CLR");
      armv7m_atomic_or(&timer()->TIM->DIER, TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC3DE);
      armv7m_atomic_modify(&timer()->TIM->DIER, TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC3DE, 0);
      
      TRACE(BLADE, "DMA");
      stm32l4_dma_start(&dma_, offset + (uint32_t)(&(GPIO->BSRR)),
			(uint32_t)&bit_,
			bits,
			DMA_OPTION_EVENT_TRANSFER_DONE |
			DMA_OPTION_MEMORY_TO_PERIPHERAL |
			DMA_OPTION_PERIPHERAL_DATA_SIZE_8 |
			DMA_OPTION_MEMORY_DATA_SIZE_8 |
			DMA_OPTION_PRIORITY_VERY_HIGH);
      if (CIRCULAR) {
	TRACE(BLADE, "circular!");
	bits_to_send_ = bits + 1;
	stm32l4_dma_start(&dma2_, offset + (uint32_t)(&(GPIO->BRR)),
			  (uint32_t)begin_,
			  bits_per_interrupt_ * 2,
			  DMA_OPTION_EVENT_TRANSFER_DONE |
			  DMA_OPTION_EVENT_TRANSFER_HALF |
			  DMA_OPTION_MEMORY_TO_PERIPHERAL |
			  DMA_OPTION_PERIPHERAL_DATA_SIZE_8 |
			  DMA_OPTION_MEMORY_DATA_SIZE_8 |
			  DMA_OPTION_MEMORY_DATA_INCREMENT |
			  DMA_OPTION_PRIORITY_VERY_HIGH |
			  DMA_OPTION_CIRCULAR);
      } else {
	TRACE(BLADE, "linear");
	stm32l4_dma_start(&dma2_, offset + (uint32_t)(&(GPIO->BRR)),
			  (uint32_t)begin_,
			  bits + 1,
			  DMA_OPTION_EVENT_TRANSFER_DONE |
			  DMA_OPTION_MEMORY_TO_PERIPHERAL |
			  DMA_OPTION_PERIPHERAL_DATA_SIZE_8 |
			  DMA_OPTION_MEMORY_DATA_SIZE_8 |
			  DMA_OPTION_MEMORY_DATA_INCREMENT |
			  DMA_OPTION_PRIORITY_VERY_HIGH);
      }
      stm32l4_dma_start(&dma3_, offset + (uint32_t)(&(GPIO->BRR)),
			(uint32_t)&bit_,
			bits + 1,
			DMA_OPTION_EVENT_TRANSFER_DONE |
			DMA_OPTION_MEMORY_TO_PERIPHERAL |
			DMA_OPTION_PERIPHERAL_DATA_SIZE_8 |
			DMA_OPTION_MEMORY_DATA_SIZE_8 |
			DMA_OPTION_PRIORITY_VERY_HIGH);

      
      TRACE(BLADE, "GPIO");
      stm32l4_gpio_pin_configure(g_APinDescription[pin].pin,
				 (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_OUTPUT));
      
      TRACE(BLADE, "TRIGGER");
      // Trigger DMA on update
      armv7m_atomic_or(&timer()->TIM->DIER, TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC3DE);
      
      TRACE(BLADE, "START!");
      noInterrupts();
      stm32l4_timer_start(timer(), false);
      interrupts();
      TRACE(BLADE, "running!");
//      timer()->TIM->EGR = TIM_EGR_UG;
      
    } else {
      TRACE(BLADE, "timer");
      client->set01(client->get_t0h(), client->get_t1h());
      Fill(false);
      
      // Enabling dma sometimes triggers an interrupt, this helps us ignore it.
      stm32l4_dma_enable(&dma_, &dma_done_callback_ignore, (void*)NULL);
      if (CIRCULAR) {
	stm32l4_dma_enable(&dma_, &dma_refill_callback1, (void*)this);
      } else {
	stm32l4_dma_enable(&dma_, &dma_done_callback, (void*)this);
      }
      
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

      if (CIRCULAR) {
	TRACE(BLADE, "circular!");
	stm32l4_dma_start(&dma_, (uint32_t)cmp_address,
			  (uint32_t)begin_,
			  bits_per_interrupt_ * 2,
			  DMA_OPTION_EVENT_TRANSFER_DONE |
			  DMA_OPTION_EVENT_TRANSFER_HALF |
			  DMA_OPTION_MEMORY_TO_PERIPHERAL |
			  DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
			  DMA_OPTION_MEMORY_DATA_SIZE_8 |
			  DMA_OPTION_MEMORY_DATA_INCREMENT |
			  DMA_OPTION_PRIORITY_HIGH |
			  DMA_OPTION_CIRCULAR);
	bits_to_send_ = bits + 1;
      } else {
	TRACE(BLADE, "linear");
	stm32l4_dma_start(&dma_, (uint32_t)cmp_address,
			  (uint32_t)begin_,
			  bits + 1,
			  DMA_OPTION_EVENT_TRANSFER_DONE |
			  DMA_OPTION_MEMORY_TO_PERIPHERAL |
			  DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
			  DMA_OPTION_MEMORY_DATA_SIZE_8 |
			  DMA_OPTION_MEMORY_DATA_INCREMENT |
			  DMA_OPTION_PRIORITY_HIGH);
      }
      
      
      armv7m_atomic_or(&timer()->TIM->CR1, TIM_CR1_ARPE);
      
      stm32l4_timer_start(timer(), false);
      stm32l4_timer_channel(timer(), g_APinDescription[pin].pwm_channel, 0, TIMER_CONTROL_PWM);
      stm32l4_gpio_pin_configure(g_APinDescription[pin].pin,
				 (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
      
      // Trigger DMA on update
      armv7m_atomic_or(&timer()->TIM->DIER, TIM_DIER_UDE);
    }
    TRACE(BLADE, "show exit");

    extern void ClockControl_AvoidSleep();
    ClockControl_AvoidSleep();
  }
  
  void DoRefill1() {
    TRACE(BLADE, "DoRefill1");
    sent_ += bits_per_interrupt_;
    if (sent_ > bits_to_send_) {
      DoDoneCB();
    } else {
      Fill(stm32l4_dma_count(&dma_) >= (half_ - begin_));
    }
  }
  void DoRefill2() {
    TRACE(BLADE, "DoRefill2");
    // Done callback will be called by the other dma.
    Fill(stm32l4_dma_count(&dma2_) >= (half_ - begin_));
  }
  
  static void dma_refill_callback2(void* context, uint32_t events) {
    ScopedCycleCounter cc(pixel_dma_interrupt_cycles);
    ((WS2811EngineSTM32L4*)context)->DoRefill2();
  }
  static void dma_refill_callback1(void* context, uint32_t events) {
    ScopedCycleCounter cc(pixel_dma_interrupt_cycles);
    ((WS2811EngineSTM32L4*)context)->DoRefill1();
  }
  static void dma_done_callback_ignore(void* context, uint32_t events) {}
  static void dma_done_callback(void* context, uint32_t events) {
    ScopedCycleCounter cc(pixel_dma_interrupt_cycles);
    ((WS2811EngineSTM32L4*)context)->DoDoneCB();
  }
  void DoDoneCB() {
    TRACE(BLADE, "dma done enter");
    // Set the pin to low, normal output mode. This will keep the pin low even if we
    // re-use the timer for another show() call.
    digitalWrite(pin_, LOW);

    stm32l4_timer_stop(timer());
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
    // GPIO A/B/C may sleep on WFE now.
    RCC->AHB2SMENR &= ~(RCC_AHB2SMENR_GPIOASMEN | RCC_AHB2SMENR_GPIOBSMEN | RCC_AHB2SMENR_GPIOCSMEN);
    client_->done_callback();
    noInterrupts();
    client_ = client_->next_ws2811_client_;
    PROFFIEOS_ASSERT( !client_ || color_buffer_size );
    interrupts();
    armv7m_pendsv_enqueue((armv7m_pendsv_routine_t)static_kick, (void *)this, 0);
    TRACE(BLADE, "dma done exit");
  }

  static void static_kick(void *context) {
    WS2811EngineSTM32L4* engine = ((WS2811EngineSTM32L4*)context);
    engine->kick();
  }
  
private:
  static uint8_t* begin_;
  static uint8_t* half_;
  static uint8_t* end_; 
  static uint8_t* dest_;
  static WS2811Client* volatile client_;
  static WS2811Client* volatile last_client_;
  static uint32_t chunk_bits_;
  static uint32_t chunk_size_;
  static uint32_t bits_per_interrupt_;

  static uint32_t sent_;

  static uint32_t bits_to_send_;
  static uint32_t read_calls_;

  static int pin_;
  static volatile uint8_t bit_;
  static stm32l4_timer_t timer_;
  static stm32l4_dma_t dma_;
  static stm32l4_dma_t dma2_;
  static stm32l4_dma_t dma3_;
};

WS2811Client* volatile WS2811EngineSTM32L4::client_;
WS2811Client* volatile WS2811EngineSTM32L4::last_client_;
uint32_t WS2811EngineSTM32L4::chunk_bits_;
uint32_t WS2811EngineSTM32L4::chunk_size_;
uint32_t WS2811EngineSTM32L4::bits_per_interrupt_;
uint32_t WS2811EngineSTM32L4::sent_;
uint32_t WS2811EngineSTM32L4::bits_to_send_;
uint32_t WS2811EngineSTM32L4::read_calls_;
int WS2811EngineSTM32L4::pin_;
uint8_t* WS2811EngineSTM32L4::begin_;
uint8_t* WS2811EngineSTM32L4::end_;
uint8_t* WS2811EngineSTM32L4::half_;
uint8_t* WS2811EngineSTM32L4::dest_;
stm32l4_timer_t WS2811EngineSTM32L4::timer_;
stm32l4_dma_t WS2811EngineSTM32L4::dma_;
stm32l4_dma_t WS2811EngineSTM32L4::dma2_;
stm32l4_dma_t WS2811EngineSTM32L4::dma3_;
volatile uint8_t WS2811EngineSTM32L4::bit_;


WS2811Engine* GetWS2811Engine(int pin) {
  static WS2811EngineSTM32L4 engine;
  if (pin < 0) return nullptr;
  return &engine;
}

// Usage:
// beginframe
// fill color buffer
// endframe
template<Color8::Byteorder BYTEORDER>
class WS2811PinBase : public WS2811Client, public WS2811PIN {
public:
  // Note, t0h_us/t1h_us are out of 1250 us, even if the frequency is
  // not 800kHz. If you're reading the datasheet for a 400Hz strip, you
  // would need to divide by two.
  WS2811PinBase(int num_leds,
		int8_t pin,
		int frequency,
		int reset_us,
		int t0h_us = 294,
		int t1h_us = 862) {
    pin_ = pin;
    engine_ = GetWS2811Engine(pin);
    num_leds_ = num_leds;
    frequency_ = frequency;
    reset_us_ = reset_us;
    int pulse_len = timer_frequency / frequency;
    t0h_ = pulse_len * t0h_us / 1250;
    t1h_ = pulse_len * t1h_us / 1250;
#if 0    
    if (!engine_) {
      Serial.print("Pin ");
      Serial.print(pin);
      Serial.println(" cannot do WS2811 output.");
    }
#endif    
  }

  bool IsReadyForBeginFrame() override {
    if (num_leds_ > NELEM(color_buffer)) {
      STDOUT.print("Display memory is not big enough, increase maxLedsPerStrip!");
      return false;
    }
    return NELEM(color_buffer) - color_buffer_size >= num_leds_;
  }

  Color16* BeginFrame() override {
    TRACE(BLADE, "beginframe enter");
    while (!IsReadyForBeginFrame()) armv7m_core_yield();
    TRACE(BLADE, "exit");

    noInterrupts();
    Color16 *ret = color_buffer_ptr + color_buffer_size;
    interrupts();

    if (ret >= color_buffer + NELEM(color_buffer)) ret -= NELEM(color_buffer);
    return ret;
  }

  bool IsReadyForEndFrame() override {
    return done_ && (micros() - done_time_us_) > reset_us_;
  }

  void EndFrame() override {
    armv7m_atomic_add(&color_buffer_size, num_leds_);
    TRACE(BLADE, "endframe enter");
    if (!engine_) return;
    while (!IsReadyForEndFrame()) armv7m_core_yield();
    frame_num_++;

    if (engine_) {
      done_ = false;
      engine_->queue(this);
    }

    TRACE(BLADE, "exit");
  }

  int num_leds() const override { return num_leds_; }
  Color8::Byteorder get_byteorder() const override { return BYTEORDER; }
  void Enable(bool on) override {
    pinMode(pin_, on ? OUTPUT : INPUT_ANALOG);
  }
  int pin() const override { return pin_; }

private:
  void done_callback() override {
    done_time_us_ = micros();
    done_ = true;
  }

  void read(uint8_t* dest) override __attribute__((optimize("Ofast"))) {
    PROFFIEOS_ASSERT(color_buffer_size);
    Color16* pos = color_buffer_ptr;
    uint32_t* output = (uint32_t*) dest;
    Color8 color = pos->dither(frame_num_, pos - color_buffer);
#if 0    
    for (int i = Color8::inline_num_bytes(BYTEORDER) - 1; i >= 0; i--) {
      uint32_t tmp = color.inline_getByte(BYTEORDER, i) * 0x8040201U;
      *(output++) = zero4X_ + ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
      *(output++) = zero4X_ + ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
    }
#else
    uint32_t tmp;
    if (Color8::inline_num_bytes(BYTEORDER) == 4) {
      tmp = GETBYTE<BYTEORDER, 3>(color) * 0x8040201U;
      *(output++) = zero4X_ ^ ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
      *(output++) = zero4X_ ^ ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
    }
    tmp = GETBYTE<BYTEORDER, 2>(color) * 0x8040201U;
    *(output++) = zero4X_ ^ ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
    *(output++) = zero4X_ ^ ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
    tmp = GETBYTE<BYTEORDER, 1>(color) * 0x8040201U;
    *(output++) = zero4X_ ^ ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
    *(output++) = zero4X_ ^ ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
    tmp = GETBYTE<BYTEORDER, 0>(color) * 0x8040201U;
    *(output++) = zero4X_ ^ ((tmp >> 7) & 0x01010101U) * one_minus_zero_;
    *(output++) = zero4X_ ^ ((tmp >> 3) & 0x01010101U) * one_minus_zero_;
#endif
    pos++;
    if (pos == color_buffer + NELEM(color_buffer)) pos = color_buffer;
    armv7m_atomic_sub(&color_buffer_size, 1);
    color_buffer_ptr = pos;
  }

  int chunk_size() override {
    return Color8::num_bytes(BYTEORDER) * 8;
  }
  
  int frequency() override { return frequency_; }
  int num_leds() override { return num_leds_; }

  void set01(uint8_t zero, uint8_t one) override {
    zero4X_ = zero * 0x01010101;
    one_minus_zero_ = one ^ zero;
  }

  uint8_t get_t0h() override { return t0h_; }
  uint8_t get_t1h() override { return t1h_; }


  WS2811Engine* engine_;
  int8_t pin_;
  uint8_t frame_num_ = 0;
  uint16_t num_leds_;
  int frequency_;
  uint32_t reset_us_;

  // These are in timer units, not us
  uint8_t t0h_;
  uint8_t t1h_;

  uint32_t one_minus_zero_;
  uint32_t zero4X_;

  volatile bool done_ = true;
  volatile uint32_t done_time_us_ = 0;
};

template<int LEDS, int PIN, Color8::Byteorder BYTEORDER, int frequency=800000, int reset_us=300, int t0h=294, int t1h=892>
class WS2811Pin : public WS2811PinBase<BYTEORDER> {
public:
  WS2811Pin() : WS2811PinBase<BYTEORDER>(LEDS, PIN, frequency, reset_us, t0h, t1h) {}
};

#endif

