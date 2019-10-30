#ifndef SOUND_DAC_H
#define SOUND_DAC_H

// DMA-driven audio output.
// Based on the Teensy Audio library code.
#define AUDIO_BUFFER_SIZE 44
#define AUDIO_RATE 44100

#ifdef USE_I2S

#ifdef TEENSYDUINO
// MCLK needs to be 48e6 / 1088 * 256 = 11.29411765 MHz -> 44.117647 kHz sample rate
//
#if F_CPU == 96000000 || F_CPU == 48000000 || F_CPU == 24000000
  // PLL is at 96 MHz in these modes
  #define MCLK_MULT 2
  #define MCLK_DIV  17
#elif F_CPU == 72000000
  #define MCLK_MULT 8
  #define MCLK_DIV  51
#elif F_CPU == 120000000
  #define MCLK_MULT 8
  #define MCLK_DIV  85
#elif F_CPU == 144000000
  #define MCLK_MULT 4
  #define MCLK_DIV  51
#elif F_CPU == 168000000
  #define MCLK_MULT 8
  #define MCLK_DIV  119
#elif F_CPU == 180000000
  #define MCLK_MULT 16
  #define MCLK_DIV  255
  #define MCLK_SRC  0
#elif F_CPU == 192000000
  #define MCLK_MULT 1
  #define MCLK_DIV  17
#elif F_CPU == 216000000
  #define MCLK_MULT 8
  #define MCLK_DIV  153
  #define MCLK_SRC  0
#elif F_CPU == 240000000
  #define MCLK_MULT 4
  #define MCLK_DIV  85
#elif F_CPU == 16000000
  #define MCLK_MULT 12
  #define MCLK_DIV  17
#else
  #error "This CPU Clock Speed is not supported.";
#endif

#ifndef MCLK_SRC
#if F_CPU >= 20000000
  #define MCLK_SRC  3  // the PLL
#else
  #define MCLK_SRC  0  // system clock
#endif
#endif

#define CHANNELS 2

#else // TEENSYDUINO

#define CHANNELS 1

#endif

#else   // USE_I2S
#define CHANNELS 1
#endif  // USE_I2S

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_CONT | PDB_SC_PDBIE | PDB_SC_DMAEN)

class LS_DAC : CommandParser, Looper {
public:
  virtual void Loop() override {}
  virtual const char* name() { return "DAC"; }
  void Setup() override {
    if (!needs_setup_) return;
    needs_setup_ = false;
#ifdef TEENSYDUINO
    dma.begin(true); // Allocate the DMA channel first

#ifdef USE_I2S
    SIM_SCGC6 |= SIM_SCGC6_I2S;
    SIM_SCGC7 |= SIM_SCGC7_DMA;
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX;

    // enable MCLK output
    I2S0_MCR = I2S_MCR_MICS(MCLK_SRC) | I2S_MCR_MOE;
    while (I2S0_MCR & I2S_MCR_DUF) ;
    I2S0_MDR = I2S_MDR_FRACT((MCLK_MULT-1)) | I2S_MDR_DIVIDE((MCLK_DIV-1));

    // configure transmitter
    I2S0_TMR = 0;
    I2S0_TCR1 = I2S_TCR1_TFW(1);  // watermark at half fifo size
    I2S0_TCR2 = I2S_TCR2_SYNC(0) | I2S_TCR2_BCP | I2S_TCR2_MSEL(1)
      | I2S_TCR2_BCD | I2S_TCR2_DIV(3);
    I2S0_TCR3 = I2S_TCR3_TCE;
    I2S0_TCR4 = I2S_TCR4_FRSZ(1) | I2S_TCR4_SYWD(15) | I2S_TCR4_MF
      | I2S_TCR4_FSE | I2S_TCR4_FSP | I2S_TCR4_FSD;
    I2S0_TCR5 = I2S_TCR5_WNW(15) | I2S_TCR5_W0W(15) | I2S_TCR5_FBT(15);

    // configure pin mux for 3 clock signals
    CORE_PIN23_CONFIG = PORT_PCR_MUX(6); // pin 23, PTC2, I2S0_TX_FS (LRCLK)
    CORE_PIN9_CONFIG  = PORT_PCR_MUX(6); // pin  9, PTC3, I2S0_TX_BCLK
    CORE_PIN22_CONFIG = PORT_PCR_MUX(6); // pin 22, PTC1, I2S0_TXD0

#if defined(KINETISK)
    dma.TCD->SADDR = dac_dma_buffer;
    dma.TCD->SOFF = 2;
    dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
    dma.TCD->NBYTES_MLNO = 2;
    dma.TCD->SLAST = -sizeof(dac_dma_buffer);
    dma.TCD->DADDR = &I2S0_TDR0;
    dma.TCD->DOFF = 0;
    dma.TCD->CITER_ELINKNO = sizeof(dac_dma_buffer) / 2;
    dma.TCD->DLASTSGA = 0;
    dma.TCD->BITER_ELINKNO = sizeof(dac_dma_buffer) / 2;
    dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
#endif
    dma.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_TX);
    dma.enable();
    
    I2S0_TCSR = I2S_TCSR_SR;
    I2S0_TCSR = I2S_TCSR_TE | I2S_TCSR_BCE | I2S_TCSR_FRDE;

#else   // USE_I2S

    SIM_SCGC2 |= SIM_SCGC2_DAC0;
    DAC0_C0 = DAC_C0_DACEN;
#ifdef LOUD
    DAC0_C0 |= DAC_C0_DACRFS;  // 3.3V, much louder
#endif
    // This would cause a click, but the amp is not on yet...
    *(int16_t *)&(DAC0_DAT0L) = 2048;

    // set the programmable delay block to trigger DMA requests
    SIM_SCGC6 |= SIM_SCGC6_PDB;
    PDB0_IDLY = 1;
    PDB0_MOD = F_BUS / AUDIO_RATE;
    PDB0_SC = PDB_CONFIG | PDB_SC_LDOK;
    PDB0_SC = PDB_CONFIG | PDB_SC_SWTRIG;
    PDB0_CH0C1 = 0x0101;

    dma.TCD->SADDR = dac_dma_buffer;
    dma.TCD->SOFF = 2;
    dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
    dma.TCD->NBYTES_MLNO = 2;
    dma.TCD->SLAST = -sizeof(dac_dma_buffer);
    dma.TCD->DADDR = &DAC0_DAT0L;
    dma.TCD->DOFF = 0;
    dma.TCD->CITER_ELINKNO = NELEM(dac_dma_buffer);
    dma.TCD->DLASTSGA = 0;
    dma.TCD->BITER_ELINKNO = NELEM(dac_dma_buffer);
    dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
    dma.triggerAtHardwareEvent(DMAMUX_SOURCE_PDB);
    dma.enable();
#endif
    dma.attachInterrupt(isr);

#else  // teensyduino
    // check return value?
    stm32l4_dma_create(&dma, DMA_CHANNEL_DMA2_CH6_SAI1_A, STM32L4_SAI_IRQ_PRIORITY);
#endif
  }

  void begin() {
    if (on_) return;
    on_ = true;
    Setup();

    memset(dac_dma_buffer, 0, sizeof(dac_dma_buffer));
#ifndef TEENSYDUINO
    stm32l4_system_periph_enable(SYSTEM_PERIPH_SAI1);
    stm32l4_dma_enable(&dma, &isr, 0);
    SAI_Block_TypeDef *SAIx = SAI1_Block_A;
    SAIx->CR1 = SAI_xCR1_DS_2 | SAI_xCR1_MONO | SAI_xCR1_CKSTR;
    SAIx->FRCR = (31 << SAI_xFRCR_FRL_Pos) | (15 << SAI_xFRCR_FSALL_Pos) | SAI_xFRCR_FSDEF | SAI_xFRCR_FSOFF;
    SAIx->SLOTR = SAI_xSLOTR_NBSLOT_0 | (0x0003 << SAI_xSLOTR_SLOTEN_Pos) | SAI_xSLOTR_SLOTSZ_0;
    stm32l4_system_saiclk_configure(SYSTEM_SAICLK_11289600);
    extern const stm32l4_sai_pins_t g_SAIPins;
    stm32l4_gpio_pin_configure(g_SAIPins.sck, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_gpio_pin_configure(g_SAIPins.fs, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_gpio_pin_configure(g_SAIPins.sd, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_dma_start(&dma, (uint32_t)&SAIx->DR, (uint32_t)dac_dma_buffer, AUDIO_BUFFER_SIZE * 2,
                      DMA_OPTION_EVENT_TRANSFER_DONE |
                      DMA_OPTION_EVENT_TRANSFER_HALF |
                      DMA_OPTION_MEMORY_TO_PERIPHERAL |
                      DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
                      DMA_OPTION_MEMORY_DATA_SIZE_16 |
                      DMA_OPTION_MEMORY_DATA_INCREMENT |
                      DMA_OPTION_PRIORITY_HIGH |
                      DMA_OPTION_CIRCULAR);

    SAIx->CR1 |= SAI_xCR1_DMAEN;
    if (!(SAIx->CR1 & SAI_xCR1_SAIEN))
    {
      SAIx->CR2 = SAI_xCR2_FTH_1;
      SAIx->CR1 |= SAI_xCR1_SAIEN;
    }
#endif
  }

  void end() {
    if (!on_) return;
    on_ = false;
#ifndef TEENSYDUINO
    SAI_Block_TypeDef *SAIx = SAI1_Block_A;

    SAIx->CR1 &=~ SAI_xCR1_SAIEN;
    // Poll until actually off
    while (SAIx->CR1 & SAI_xCR1_SAIEN);
    
    stm32l4_system_saiclk_configure(SYSTEM_SAICLK_NONE);

    stm32l4_dma_disable(&dma);
    stm32l4_system_periph_disable(SYSTEM_PERIPH_SAI1);
    extern const stm32l4_sai_pins_t g_SAIPins;
    stm32l4_gpio_pin_configure(g_SAIPins.sck, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
    stm32l4_gpio_pin_configure(g_SAIPins.fs, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
    stm32l4_gpio_pin_configure(g_SAIPins.sd, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
#endif
  }

  bool Parse(const char* cmd, const char* arg) override {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "dacbuf")) {
#ifndef TEENSYDUINO
      SAI_Block_TypeDef *SAIx = SAI1_Block_A;
      STDOUT.print("STATUS: ");
      STDOUT.print(SAIx->SR, HEX);
      STDOUT.print(" CR1: ");
      STDOUT.print(SAIx->CR1, HEX);
      STDOUT.print(" CR2: ");
      STDOUT.println(SAIx->CR2, HEX);
#endif      
      STDOUT.print("Current position: ");
      STDOUT.println(((uint16_t*)current_position()) - dac_dma_buffer);
      for (size_t i = 0; i < NELEM(dac_dma_buffer); i++) {
#ifdef TEENSYDUINO	
        STDOUT.print(dac_dma_buffer[i] - 2048);
#else	
        STDOUT.print(dac_dma_buffer[i]);
#endif	
        if ((i & 0xf) == 0xf)
          STDOUT.println("");
        else
          STDOUT.print(" ");
      }
      STDOUT.println("");
      return true;
    }
#endif    
    return false;
  }

  bool isSilent() {
     for (size_t i = 0; i < NELEM(dac_dma_buffer); i++)
       if (dac_dma_buffer[i] != dac_dma_buffer[0])
         return false;
     return true;
  }

  void Help() override {
    STDOUT.println(" dacbuf - print the current contents of the dac buffer");
  }

  void SetStream(class AudioStream* stream) {
    stream_ = stream;
  }

private:
  static uint32_t current_position() {
#ifdef TEENSYDUINO
    return (uint32_t)(dma.TCD->SADDR);
#else
    return (uint32_t)(dac_dma_buffer + stm32l4_dma_count(&dma));
#endif
  }
  // Interrupt handler.
  // Fills the dma buffer with new sample data.
#ifdef TEENSYDUINO
  static void isr(void)
#else
  static void isr(void* arg, unsigned long int event)
#endif
  {
    ScopedCycleCounter cc(audio_dma_interrupt_cycles);
    int16_t *dest, *end;
    uint32_t saddr = current_position();

#ifdef TEENSYDUINO
    dma.clearInterrupt();
#endif
    if (saddr < (uint32_t)dac_dma_buffer + sizeof(dac_dma_buffer) / 2) {
      // DMA is transmitting the first half of the buffer
      // so we must fill the second half
      dest = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*CHANNELS];
      end = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];
    } else {
      // DMA is transmitting the second half of the buffer
      // so we must fill the first half
      dest = (int16_t *)dac_dma_buffer;
      end = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*CHANNELS];
    }
    AudioStream *stream = stream_;
    int16_t data[AUDIO_BUFFER_SIZE];
    int n = 0;
    if (stream) {
      n = stream->read(data, (end-dest) / CHANNELS);
    }
    while (n < AUDIO_BUFFER_SIZE) data[n++] = 0;
    for (int i = 0; i < n; i++) {
#ifdef USE_I2S
#if CHANNELS == 2
      // Duplicate sample to left and right channel.
      *(dest++) = data[i];
#endif      
      *(dest++) = data[i];
#else
      *(dest++) = (((uint16_t*)data)[i] + 32768) >> 4;
#endif
    }
  }

  bool on_ = false;
  bool needs_setup_ = true;
  DMAMEM static uint16_t dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];
  static AudioStream * volatile stream_;
  static DMAChannel dma;
};

#ifdef TEENSYDUINO
DMAChannel LS_DAC::dma(false);
#else
DMAChannel LS_DAC::dma;
#endif  
AudioStream * volatile LS_DAC::stream_ = nullptr;
DMAMEM uint16_t LS_DAC::dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];

LS_DAC dac;

#endif
