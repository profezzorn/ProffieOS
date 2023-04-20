#ifndef COMMON_ANALOG_READ_H
#define COMMON_ANALOG_READ_H

// AnalogReader is a class for async analogRead calls.

#ifdef STM32_H

#include "stm32l4_adc.h"
#include "stm32l4_gpio.h"
#include "stm32l4_system.h"

#define ADC_SAMPLE_TIME_2_5    0
#define ADC_SAMPLE_TIME_6_5    1
#define ADC_SAMPLE_TIME_12_5   2
#define ADC_SAMPLE_TIME_24_5   3
#define ADC_SAMPLE_TIME_47_5   4
#define ADC_SAMPLE_TIME_92_5   5
#define ADC_SAMPLE_TIME_247_5  6
#define ADC_SAMPLE_TIME_640_5  7

bool PO_stm32l4_adc_calibrate(stm32l4_adc_t *adc)
{
    ADC_TypeDef *ADCx = adc->ADCx;

    if (adc->state != ADC_STATE_READY)
    {
	return false;
    }

    ADCx->CR |= ADC_CR_ADDIS;

    while (ADCx->CR & ADC_CR_ADEN)
    {
    }

    armv7m_core_udelay(1);

    /* Single-Ended Input Calibration */
    ADCx->CR &= ~ADC_CR_ADCALDIF;
    ADCx->CR |= ADC_CR_ADCAL;
    
    while (ADCx->CR & ADC_CR_ADCAL)
    {
    }

#if 0    
    /* Differential Input Calibration */
    ADCx->CR |= (ADC_CR_ADCALDIF | ADC_CR_ADCAL);

    while (ADCx->CR & ADC_CR_ADCAL)
    {
    }
#endif

    armv7m_core_udelay(100);

    ADCx->ISR = ADC_ISR_ADRDY;

    do
    {
	ADCx->CR |= ADC_CR_ADEN;
    }
    while (!(ADCx->ISR & ADC_ISR_ADRDY));

    return true;
}


class AnalogReader {
public:
  // Charge time specifies the minimum time to charge the builtin signal hold capacitor.
  explicit AnalogReader(int pin, int pin_mode = INPUT, float charge_time = -1.0) : pin_(pin), pin_mode_(pin_mode) {
    int channel = g_APinDescription[pin].adc_input;
    if (charge_time < 0.0) {
      charge_time = 500e-9; // default is 500 ns.
      if (channel == ADC_CHANNEL_ADC1_TS)
	charge_time = 5e-6; // 5 us
      else if (channel == ADC_CHANNEL_ADC1_VBAT)
	charge_time = 12e-6; // 12 us
    }
    float cycles = charge_time * SystemCoreClock;
    if (cycles <= 2.5) adc_smp_ = ADC_SAMPLE_TIME_2_5;
    else if (cycles <= 6.5) adc_smp_ = ADC_SAMPLE_TIME_6_5;
    else if (cycles <= 12.5) adc_smp_ = ADC_SAMPLE_TIME_12_5;
    else if (cycles <= 24.5) adc_smp_ = ADC_SAMPLE_TIME_24_5;
    else if (cycles <= 47.5) adc_smp_ = ADC_SAMPLE_TIME_47_5;
    else if (cycles <= 92.5) adc_smp_ = ADC_SAMPLE_TIME_92_5;
    else if (cycles <= 247.5) adc_smp_ = ADC_SAMPLE_TIME_247_5;
    else adc_smp_ = ADC_SAMPLE_TIME_640_5;
    // TODO: Change ADC clock if 640.5 cycles is not enough.
  }

  
  bool Start() {
    if (stm32l4_adc.state == ADC_STATE_NONE) {
      stm32l4_adc_create(&stm32l4_adc, ADC_INSTANCE_ADC1, STM32L4_ADC_IRQ_PRIORITY, 0);
      stm32l4_adc_enable(&stm32l4_adc, 0, NULL, NULL, 0);
      PO_stm32l4_adc_calibrate(&stm32l4_adc);
      stm32l4_adc_disable(&stm32l4_adc);
    }
    if (stm32l4_adc.state != ADC_STATE_INIT)
      return false;
    current_AnalogReader = this;
    state_machine_.reset_state_machine();
    loop();
    return true;
  }

  
  void LoopUntilStart() {
    while (!Start()) current_AnalogReader->loop();
  }

  bool Done() {
    loop();
    return state_machine_.next_state_ == -2;
  }

  int Value() const {
    return value_ >> 2;
  }

  void loop() {
    STATE_MACHINE_BEGIN();
#define channel (g_APinDescription[pin_].adc_input)
#define ADCx (stm32l4_adc.ADCx)

    while (stm32l4_adc.state != ADC_STATE_INIT) YIELD();
    stm32l4_adc_enable(&stm32l4_adc, 0,NULL, NULL, 0);
    stm32l4_adc.state = ADC_STATE_BUSY;

    {
      // pullup/pulldown does NOT WORK
      uint32_t mode = GPIO_MODE_ANALOG | GPIO_ANALOG_SWITCH;
      stm32l4_gpio_pin_configure(g_APinDescription[pin_].pin, mode);
    }

    /* Silicon ERRATA 2.4.4. Wrong ADC conversion results when delay between
     * calibration and first conversion or between 2 consecutive conversions is too long. 
     */

    if (stm32l4_adc.instance == ADC_INSTANCE_ADC1) {
      if (channel == ADC_CHANNEL_ADC1_TS)
      {
	ADCx->CR |= ADC_CR_ADDIS;
	
	while (ADCx->CR & ADC_CR_ADEN) YIELD();
	
#if defined(STM32L476xx) || defined(STM32L496xx)
	armv7m_atomic_or(&ADC123_COMMON->CCR, ADC_CCR_TSEN);
#else /* defined(STM32L476xx) || defined(STM32L496xx) */
	armv7m_atomic_or(&ADC1_COMMON->CCR, ADC_CCR_TSEN);
#endif /* defined(STM32L476xx) || defined(STM32L496xx) */
	
	ADCx->ISR = ADC_ISR_ADRDY;
	
	do
	{
	  YIELD();
	  ADCx->CR |= ADC_CR_ADEN;
	}
	while (!(ADCx->ISR & ADC_ISR_ADRDY));
	
	SLEEP_MICROS(120);
      }
    }

    #if 1
    // This is a hack. We chage up the sampling capacitor from VREFINT (to 1.1 volts)
    // then we activate the input a very short time and see how much it changes the sampling
    // capacitor. It's not the same as INPUT_PULLUP, but at least it provides some sort of
    // reading.
    if (pin_mode_ == INPUT_PULLUP) {
      adc_smp_ = ADC_SAMPLE_TIME_2_5;
      ADCx->SQR1 = (ADC_CHANNEL_ADC1_VREFINT << 6);
      ADCx->SMPR1 = (ADC_CHANNEL_ADC1_VREFINT < 10) ? (adc_smp_ << (ADC_CHANNEL_ADC1_VREFINT * 3)) : 0;
      ADCx->SMPR2 = (ADC_CHANNEL_ADC1_VREFINT >= 10) ? (adc_smp_ << ((ADC_CHANNEL_ADC1_VREFINT * 3) - 30)) : 0;
      adc_smp_ = ADC_SAMPLE_TIME_2_5;
    } else {
      ADCx->SQR1 = (channel << 6);
      ADCx->SMPR1 = (channel < 10) ? (adc_smp_ << (channel * 3)) : 0;
      ADCx->SMPR2 = (channel >= 10) ? (adc_smp_ << ((channel * 3) - 30)) : 0;
    }
    #else
      ADCx->SQR1 = (channel << 6);
      ADCx->SMPR1 = (channel < 10) ? (adc_smp_ << (channel * 3)) : 0;
      ADCx->SMPR2 = (channel >= 10) ? (adc_smp_ << ((channel * 3) - 30)) : 0;
    #endif

    ADCx->CR |= ADC_CR_ADSTART;
    
    while (!(ADCx->ISR & ADC_ISR_EOC)) YIELD();

    // Ignore first read.
    value_ = ADCx->DR & ADC_DR_RDATA;
	
    ADCx->ISR = ADC_ISR_EOC;

    // Set it to the proper channel
    ADCx->SQR1 = (channel << 6);
    ADCx->SMPR1 = (channel < 10) ? (adc_smp_ << (channel * 3)) : 0;
    ADCx->SMPR2 = (channel >= 10) ? (adc_smp_ << ((channel * 3) - 30)) : 0;

    ADCx->CR |= ADC_CR_ADSTART;
    
    while (!(ADCx->ISR & ADC_ISR_EOC)) YIELD();
    
    value_ = ADCx->DR & ADC_DR_RDATA;

    ADCx->ISR = ADC_ISR_EOC;

    if ((stm32l4_adc.instance == ADC_INSTANCE_ADC1) && (channel == ADC_CHANNEL_ADC1_TS))
    {
	ADCx->CR |= ADC_CR_ADDIS;

	while (ADCx->CR & ADC_CR_ADEN) YIELD();
	
#if defined(STM32L476xx) || defined(STM32L496xx)
	armv7m_atomic_and(&ADC123_COMMON->CCR, ~ADC_CCR_TSEN);
#else /* defined(STM32L476xx) || defined(STM32L496xx) */
	armv7m_atomic_and(&ADC1_COMMON->CCR, ~ADC_CCR_TSEN);
#endif /* defined(STM32L476xx) || defined(STM32L496xx) */

	ADCx->ISR = ADC_ISR_ADRDY;

	do
	{
	  YIELD();
	  ADCx->CR |= ADC_CR_ADEN;
	}
	while (!(ADCx->ISR & ADC_ISR_ADRDY));
    }
    
    stm32l4_adc.state = ADC_STATE_READY;
    stm32l4_adc_disable(&stm32l4_adc);
    current_AnalogReader = nullptr;
    STATE_MACHINE_END();
  }

#undef ADCx
#undef channel
  
  int8_t pin_;
  int8_t pin_mode_;
  uint32_t adc_smp_;
  int value_;
  StateMachineState state_machine_;
  static AnalogReader* current_AnalogReader;
};

AnalogReader* AnalogReader::current_AnalogReader = nullptr;

#else
class AnalogReader {
public:
  explicit AnalogReader(int pin, int pin_mode_ = INPUT, float charge_time = -1) : pin_(pin) {
    pinMode(pin_, pin_mode_);
  }

  bool Start() { return true; }
  void LoopUntilStart() {}
  bool Done() { return true; }
  int Value() { return analogRead(pin_); }

private:
  int pin_;
};

#endif

int LSAnalogRead(int pin, int pinmode = INPUT) {
  AnalogReader reader(pin, pinmode);
  // if (!reader.Start()) return -1;
  reader.LoopUntilStart();
  while (!reader.Done());
  return reader.Value();
}

#endif
