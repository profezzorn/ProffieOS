#ifndef BUTTONS_STM32L4_TOUCHBUTTON_
#define BUTTONS_STM32L4_TOUCHBUTTON_

#include "../common/bitfield.h"
#include "button_base.h"

struct TSC_CR_TYPE : public BitField<uint32_t, TSC_CR_TYPE> {
  using CTPH = Field<28, 4>;
  using CTPL = Field<24, 4>;
  using SSD = Field<17, 7>;
  using SSE = Field<16, 1>;
  using SSPSC = Field<15, 1>;
  using PGPSC = Field<12, 3>;
  using MCV = Field<5, 3>;
  using IODEF = Field<5, 1>;
  using SYNC_POL = Field<3, 1>;
  using AM = Field<2, 1>;
  using START = Field<1, 1>;
  using TSCE = Field<0, 1>;
};

struct TSC_IER_TYPE : public BitField<uint32_t, TSC_IER_TYPE> {
  using MCE = Field<1, 1>;
  using EOA = Field<0, 1>;
};

struct TSC_IO_TYPE : public BitField<uint32_t, TSC_IO_TYPE> {
  using G7_IO4 = Field<27, 1>;
  using G7_IO3 = Field<26, 1>;
  using G7_IO2 = Field<25, 1>;
  using G7_IO1 = Field<24, 1>;

  using G6_IO4 = Field<23, 1>;
  using G6_IO3 = Field<22, 1>;
  using G6_IO2 = Field<21, 1>;
  using G6_IO1 = Field<20, 1>;

  using G5_IO4 = Field<19, 1>;
  using G5_IO3 = Field<18, 1>;
  using G5_IO2 = Field<17, 1>;
  using G5_IO1 = Field<16, 1>;

  using G4_IO4 = Field<15, 1>;
  using G4_IO3 = Field<14, 1>;
  using G4_IO2 = Field<13, 1>;
  using G4_IO1 = Field<12, 1>;

  using G3_IO4 = Field<11, 1>;
  using G3_IO3 = Field<10, 1>;
  using G3_IO2 = Field<9, 1>;
  using G3_IO1 = Field<8, 1>;

  using G2_IO4 = Field<7, 1>;
  using G2_IO3 = Field<6, 1>;
  using G2_IO2 = Field<5, 1>;
  using G2_IO1 = Field<4, 1>;

  using G1_IO4 = Field<3, 1>;
  using G1_IO3 = Field<2, 1>;
  using G1_IO2 = Field<2, 1>;
  using G1_IO1 = Field<0, 1>;
};

struct TSC_IOGCSR_TYPE : public BitField<uint32_t, TSC_IOGCSR_TYPE> {
  using G7S = Field<22, 1>;
  using G6S = Field<21, 1>;
  using G5S = Field<20, 1>;
  using G4S = Field<19, 1>;
  using G3S = Field<18, 1>;
  using G2S = Field<17, 1>;
  using G1S = Field<16, 1>;

  using G7E = Field<6, 1>;
  using G6E = Field<5, 1>;
  using G5E = Field<4, 1>;
  using G4E = Field<3, 1>;
  using G3E = Field<2, 1>;
  using G2E = Field<1, 1>;
  using G1E = Field<0, 1>;
};

struct TSC_TYPE {
  volatile TSC_CR_TYPE CR;
  volatile TSC_IER_TYPE IER;
  volatile TSC_IER_TYPE ICR;
  volatile TSC_IER_TYPE ISR;
  volatile TSC_IO_TYPE IOHCR;
  uint32_t reserved1;
  volatile TSC_IO_TYPE IOASCR;
  uint32_t reserved2;
  volatile TSC_IO_TYPE IOSCR;
  uint32_t reserved3;
  volatile TSC_IO_TYPE IOCCR;
  uint32_t reserved4;
  volatile TSC_IOGCSR_TYPE IOGCSR;
  volatile uint32_t IOGCR[7];
};

#define TOUCH_MAX 8192

class TouchButton : public ButtonBase {
public:
  TouchButton(enum BUTTON button, int pin, int threshold, const char* name)
    : ButtonBase(name, button),
    pin_(pin),
    threshold_(threshold) {
      armv7m_atomic_or(&RCC->AHB1ENR, 1<<16);
    }
protected:
  bool Read() override {
    return is_pushed_;
  }

#undef TSC
  TSC_TYPE* TSC = (TSC_TYPE*)TSC_BASE;

  void Update(int value) {
    if (print_next_) {
      STDOUT.print("Touch ");
      STDOUT.print(name_);
      STDOUT.print(" = ");
      STDOUT.print(value);
      STDOUT.print(" (");
      STDOUT.print(min_);
      if (min_ == TOUCH_MAX) STDOUT.print(" MAX! ");
      STDOUT.print(" - ");
      STDOUT.print(max_);
      if (max_ == TOUCH_MAX) STDOUT.print(" MAX! ");
      STDOUT.println(")");

#ifdef SPEAK_TOUCH_VALUES
      if (talkie.Empty()) {
	talkie.Say2Digits(min_);
	talkie.Say(spTO);
	talkie.Say2Digits(max_);
      }
#endif

      print_next_ = false;
      min_ = 10000000;
      max_ = 0;
    } else {
      min_ = std::min(value, min_);
      max_ = std::max(value, max_);
    }
    is_pushed_ = value < threshold_;
  }

  
  void Loop() override {
    ButtonBase::Loop();
    if (monitor.ShouldPrintMultiple(Monitoring::MonitorTouch)) {
      print_next_ = true;
    }
    STATE_MACHINE_BEGIN();
    while (true) {

      // Wait until it's our turn.
      while (current_button) YIELD();
      current_button = this;

      // Config
      TSC->CR.set(TSC_CR_TYPE::CTPH(1) |
		  TSC_CR_TYPE::CTPL(1) |
		  TSC_CR_TYPE::SSD(10) |
		  TSC_CR_TYPE::SSE(1) |
		  TSC_CR_TYPE::SSPSC(1) |
		  TSC_CR_TYPE::PGPSC(6) |
		  TSC_CR_TYPE::MCV(5) |
		  TSC_CR_TYPE::IODEF(0) |
		  TSC_CR_TYPE::SYNC_POL(0) |
		  TSC_CR_TYPE::AM(0) |
		  TSC_CR_TYPE::TSCE(1));


#if PROFFIEBOARD_VERSION == 3
      TSC->IOGCSR.set(TSC_IOGCSR_TYPE::G7E(0) |
		      TSC_IOGCSR_TYPE::G6E(0) |
		      TSC_IOGCSR_TYPE::G5E(0) |
		      TSC_IOGCSR_TYPE::G4E(0) |
		      TSC_IOGCSR_TYPE::G3E(0) |
		      TSC_IOGCSR_TYPE::G2E(0) |
		      TSC_IOGCSR_TYPE::G1E(1));

      // Cap pin is always the same for now. (PB12)
      stm32l4_gpio_pin_configure(GPIO_PIN_PB12_TSC_G1_IO1,
				 (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_OPENDRAIN | GPIO_MODE_ALTERNATE));
      TSC->IOHCR.set(TSC_IO_TYPE::G2_IO1(0));
      TSC->IOSCR.set(TSC_IO_TYPE::G2_IO1(1));
//      TSC->IOASCR.set(TSC_IO_TYPE::G2_IO4(1));

      switch (pin_) {
	case 11: // PB14
	  stm32l4_gpio_pin_configure(GPIO_PIN_PB14_TSC_G1_IO3,
				     (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
	  TSC->IOHCR.set(TSC_IO_TYPE::G2_IO3(0));
	  TSC->IOCCR.set(TSC_IO_TYPE::G2_IO3(1));
//	  TSC->IOASCR.set(TSC_IO_TYPE::G2_IO3(1));
	  break;

	case 13: // PB13
	  stm32l4_gpio_pin_configure(GPIO_PIN_PB13_TSC_G1_IO2,
				     (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
	  TSC->IOHCR.set(TSC_IO_TYPE::G2_IO2(0));
	  TSC->IOCCR.set(TSC_IO_TYPE::G2_IO2(1));
//	  TSC->IOASCR.set(TSC_IO_TYPE::G2_IO2(1));
	  break;

	case 15: // PB15
	  stm32l4_gpio_pin_configure(GPIO_PIN_PB15_TSC_G1_IO4,
				     (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
	  TSC->IOHCR.set(TSC_IO_TYPE::G2_IO4(0));
	  TSC->IOCCR.set(TSC_IO_TYPE::G2_IO4(1));
//	  TSC->IOASCR.set(TSC_IO_TYPE::G2_IO1(1));
	  break;
      }
#else      
      TSC->IOGCSR.set(TSC_IOGCSR_TYPE::G7E(0) |
		      TSC_IOGCSR_TYPE::G6E(0) |
		      TSC_IOGCSR_TYPE::G5E(0) |
		      TSC_IOGCSR_TYPE::G4E(0) |
		      TSC_IOGCSR_TYPE::G3E(0) |
		      TSC_IOGCSR_TYPE::G2E(1) |
		      TSC_IOGCSR_TYPE::G1E(0));
      // Cap pin is always the same for now. (PB7)
      stm32l4_gpio_pin_configure(GPIO_PIN_PB7_TSC_G2_IO4,
				 (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_OPENDRAIN | GPIO_MODE_ALTERNATE));
      TSC->IOHCR.set(TSC_IO_TYPE::G2_IO4(0));
      TSC->IOSCR.set(TSC_IO_TYPE::G2_IO4(1));
//      TSC->IOASCR.set(TSC_IO_TYPE::G2_IO4(1));

      switch (pin_) {
	case 21: // PB6
	  stm32l4_gpio_pin_configure(GPIO_PIN_PB6_TSC_G2_IO3,
				     (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
	  TSC->IOHCR.set(TSC_IO_TYPE::G2_IO3(0));
	  TSC->IOCCR.set(TSC_IO_TYPE::G2_IO3(1));
//	  TSC->IOASCR.set(TSC_IO_TYPE::G2_IO3(1));
	  break;

	case 23: // PB5
	  stm32l4_gpio_pin_configure(GPIO_PIN_PB5_TSC_G2_IO2,
				     (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
	  TSC->IOHCR.set(TSC_IO_TYPE::G2_IO2(0));
	  TSC->IOCCR.set(TSC_IO_TYPE::G2_IO2(1));
//	  TSC->IOASCR.set(TSC_IO_TYPE::G2_IO2(1));
	  break;

	case 22: // PB4
	  stm32l4_gpio_pin_configure(GPIO_PIN_PB4_TSC_G2_IO1,
				     (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
	  TSC->IOHCR.set(TSC_IO_TYPE::G2_IO1(0));
	  TSC->IOCCR.set(TSC_IO_TYPE::G2_IO1(1));
//	  TSC->IOASCR.set(TSC_IO_TYPE::G2_IO1(1));
	  break;
      }
#endif
      
      // Discharge
      SLEEP(1);

      TSC->CR.set(TSC_CR_TYPE::IODEF(1)); // stop discharge

      // Disable interrupts
      TSC->IER.set(TSC_IER_TYPE::EOA(0) | TSC_IER_TYPE::MCE(0));
      // Clear flags
      TSC->ICR.set(TSC_IER_TYPE::EOA(1) | TSC_IER_TYPE::MCE(1));

      // Start
      TSC->CR.set(TSC_CR_TYPE::START(1));

      // Poll
      while (!TSC->ISR.get<TSC_IER_TYPE::EOA>()) YIELD();

      if (TSC->ISR.get<TSC_IER_TYPE::MCE>()) {
	// Overflow
	Update(TOUCH_MAX);
      } else {
	Update(TSC->IOGCR[1]);
      }

      TSC->IOCCR.set(TSC_IO_TYPE::G2_IO1(0) |
		     TSC_IO_TYPE::G2_IO2(0) |
		     TSC_IO_TYPE::G2_IO3(0) |
		     TSC_IO_TYPE::G2_IO4(0));
      
      // Let someone else have a turn.
      current_button = NULL;
      YIELD();
    }
    STATE_MACHINE_END();
  }
    
  static TouchButton *current_button;
  bool print_next_ = false;
  uint8_t pin_;
  int threshold_;
  int min_ = 100000000;
  int max_ = 0;
  bool is_pushed_ = false;

  TSC_TYPE* TSC_ = (TSC_TYPE*)TSC_BASE;
  TSC_TypeDef* TSCx_ = (TSC_TypeDef*)TSC_BASE;

  StateMachineState state_machine_;
};

TouchButton* TouchButton::current_button = NULL;

#endif
