#ifndef IR_STM32_IR_H
#define IR_STM32_IR_H

#ifdef PROFFIEBOARD_VERSION

class IrTransmitterSTM32 : public IRSender, public Looper {
public:
  const char* name() override { return "STM32 IR Sender"; }
  int pin() const {
#if PROFFIEBOARD_VERSION == 1
    return 6; // PB8
#elif PROFFIEBOARD_VERSION == 2
    return 19;  // PB8
#elif PROFFIEBOARD_VERSION == 3
    return 21;  // PB6
#else
#error Unsupported IR hardware.
#endif    
  }
  
  stm32l4_timer_t* timer() {
    return &stm32l4_pwm[3];
  }
  
  IrTransmitterSTM32() {
    stm32l4_timer_create(timer(), TIMER_INSTANCE_TIM16, STM32L4_PWM_IRQ_PRIORITY, 0);
#if PROFFIEBOARD_VERSION == 3
    stm32l4_dma_create(&dma_, DMA_CHANNEL_DMA1_CH3_TIM16_UP, STM32L4_PWM_IRQ_PRIORITY);
#else
    stm32l4_dma_create(&dma_, DMA_CHANNEL_DMA1_CH6_TIM16_UP, STM32L4_PWM_IRQ_PRIORITY);
#endif

    digitalWrite(pin(), LOW);
    stm32l4_gpio_pin_configure(g_APinDescription[pin()].pin,
			       (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_OUTPUT));
  }
  ~IrTransmitterSTM32() {
    stm32l4_timer_destroy(timer());
    stm32l4_dma_destroy(&dma_);
  }



  void StartBurst() {
    compact(); // sent_ should now be zero
    STDOUT << "IR: sending " << to_send_ << " pulses.";
    int pulse_len = 3;
    int divider = stm32l4_timer_clock(timer()) / (frequency_ * pulse_len);
    stm32l4_timer_enable(timer(),
			 divider -1,
			 pulse_len -1,
			 0 /* TIMER_OPTION_COUNT_PRELOAD */, NULL, NULL, 0);
    stm32l4_dma_enable(&dma_, &dma_done_callback, (void*)this);
    stm32l4_timer_stop(timer());
    timer()->TIM->CNT = 0;
    timer()->TIM->RCR = 0;
    timer()->TIM->DCR = 0xC /* TIM_DMABase_RCR */ | 0x100 /* TIM_DMABurstLength_2Transfers */;
    
    armv7m_atomic_modify(&timer()->TIM->DIER, TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC3DE, 0);
    
    stm32l4_dma_start(&dma_, (uint32_t)(&timer()->TIM->DMAR),
		      (uint32_t)(data_),
		      to_send_,
		      DMA_OPTION_EVENT_TRANSFER_DONE |
		      DMA_OPTION_MEMORY_TO_PERIPHERAL |
		      DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
		      DMA_OPTION_MEMORY_DATA_SIZE_8 |
		      DMA_OPTION_MEMORY_DATA_INCREMENT |
		      DMA_OPTION_PRIORITY_HIGH);
    sent_ = to_send_;
    
    armv7m_atomic_or(&timer()->TIM->CR1, TIM_CR1_ARPE);
    
    stm32l4_timer_start(timer(), false);
    stm32l4_timer_channel(timer(), g_APinDescription[pin()].pwm_channel, 0, TIMER_CONTROL_PWM);
    stm32l4_gpio_pin_configure(g_APinDescription[pin()].pin,
			       (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    // Trigger DMA on update
    armv7m_atomic_or(&timer()->TIM->DIER, TIM_DIER_UDE);
    TRACE(IR, "show exit");
  }

  static void dma_done_callback(void* context, uint32_t events) {
    ((IrTransmitterSTM32*)context)->dma_done_callback2(events);
  }

  void dma_done_callback2(uint32_t events) {
    TRACE(IR, "dma done enter");
    // Set the pin to low, normal output mode. This will keep the pin low even if we
    // re-use the timer for another show() call.
    digitalWrite(pin(), LOW);
    stm32l4_gpio_pin_configure(g_APinDescription[pin()].pin,
			       (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_OUTPUT));
    stm32l4_timer_stop(timer());
    stm32l4_timer_disable(timer());
    stm32l4_dma_stop(&dma_);
    stm32l4_dma_disable(&dma_);
    sending_ = false;
    TRACE(IR, "dma done exit");
  }
  
  void compact() {
    while (sending_) armv7m_core_yield();
    if (sent_ > 0) {
      if (pos_ > sent_) {
	memmove(data_, data_ + sent_, pos_ - sent_);
      }
      pos_ -= sent_;
      to_send_ -= sent_;
      sent_ = 0;
    }
  }

  void trysend() {
    if (to_send_ > sent_ && !sending_) {
      compact();
      StartBurst();
    }
  }

  void WaitForSpace() {
    if (pos_ < NELEM(data_)) return;  // We have space
    trysend();
    if (pos_ < NELEM(data_)) return;  // Sending codes helped
    compact();
    if (pos_ < NELEM(data_)) return;  // Waiting for compacting helped.
    STDOUT << "IR signal too long!\n";
    pos_ = 0;
  }

  // IRInterface
  virtual void signal(bool high, uint32_t us) override {
//    STDOUT << "SIGNAL " << high << " for " << us << " pos = " << pos_ << "\n";;
    int n = us * frequency_ / 1000000;
    while (n) {
      int x = std::min(n, 256);
      n -= x;
      WaitForSpace();
      data_[pos_++] = x - 1;
      data_[pos_++] = high ? 1 : 0;
    }
  }

  void Loop() override {
    trysend();
  }

  void send() override {
    signal(false, 10000); // 10ms of nothing
    signal(false, 27);    // padding
    signal(false, 27);    // padding
    to_send_ = pos_;
  }

private:
  int frequency_ = 38000;
  volatile bool sending_ = false;
  uint16_t to_send_ = 0;
  uint16_t sent_ = 0;
  uint16_t pos_ = 0;
  uint8_t data_[256];
  stm32l4_timer_t timer_;
  stm32l4_dma_t dma_;
};

// If nobody ever access this function, the linker can drop the global object.
IRSender* GetIRSender() {
  static IrTransmitterSTM32 sender;
  return &sender;
}

#endif
#endif
