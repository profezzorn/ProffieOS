#ifndef SOUND_DAC_H
#define SOUND_DAC_H

#include "filter.h"

#ifdef ESP32
#include "dac_esp32.h"
#else


#if defined(__IMXRT1062__)
#define IMXRT_CACHE_ENABLED 2 // 0=disabled, 1=WT, 2= WB

#include <Arduino.h>
#include <imxrt.h>

FLASHMEM
void my_set_audioClock(int nfact, int32_t nmult, uint32_t ndiv, bool force) // sets PLL4
{
	if (!force && (CCM_ANALOG_PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_ENABLE)) return;

	CCM_ANALOG_PLL_AUDIO = CCM_ANALOG_PLL_AUDIO_BYPASS | CCM_ANALOG_PLL_AUDIO_ENABLE
			     | CCM_ANALOG_PLL_AUDIO_POST_DIV_SELECT(2) // 2: 1/4; 1: 1/2; 0: 1/1
			     | CCM_ANALOG_PLL_AUDIO_DIV_SELECT(nfact);

	CCM_ANALOG_PLL_AUDIO_NUM   = nmult & CCM_ANALOG_PLL_AUDIO_NUM_MASK;
	CCM_ANALOG_PLL_AUDIO_DENOM = ndiv & CCM_ANALOG_PLL_AUDIO_DENOM_MASK;

	CCM_ANALOG_PLL_AUDIO &= ~CCM_ANALOG_PLL_AUDIO_POWERDOWN;//Switch on PLL
	while (!(CCM_ANALOG_PLL_AUDIO & CCM_ANALOG_PLL_AUDIO_LOCK)) {}; //Wait for pll-lock

	const int div_post_pll = 1; // other values: 2,4
	CCM_ANALOG_MISC2 &= ~(CCM_ANALOG_MISC2_DIV_MSB | CCM_ANALOG_MISC2_DIV_LSB);
	if(div_post_pll>1) CCM_ANALOG_MISC2 |= CCM_ANALOG_MISC2_DIV_LSB;
	if(div_post_pll>3) CCM_ANALOG_MISC2 |= CCM_ANALOG_MISC2_DIV_MSB;

	CCM_ANALOG_PLL_AUDIO &= ~CCM_ANALOG_PLL_AUDIO_BYPASS;//Disable Bypass
}


#endif

#ifdef USE_I2S

#ifdef TEENSYDUINO
// MCLK needs to be 48e6 / 1088 * 256 = 11.29411765 MHz -> 44.117647 kHz sample rate
//
#if defined(KINETISK) || defined(KINETISL)

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

#endif // KINETISK/L

#define CHANNELS 2

#elif defined(ARDUINO_ARCH_STM32L4)

#define CHANNELS 1


void my_stm32l4_system_saiclk_configure_22579200()
{
  RCC->PLLSAI1CFGR &= ~RCC_PLLSAI1CFGR_PLLSAI1PEN;
  RCC->CR &= ~RCC_CR_PLLSAI1ON;

  /* Wait till the PLLSAI1 is turned off */
  while (RCC->CR & RCC_CR_PLLSAI1RDY) {}

  RCC->PLLSAI1CFGR = ((11 << 27) | (62 << 8) | RCC_PLLSAI1CFGR_PLLSAI1PEN);
  RCC->CR |= RCC_CR_PLLSAI1ON;

  /* Wait till the PLLSAI1 is ready */
  while (!(RCC->CR & RCC_CR_PLLSAI1RDY)){}
}

#elif defined(ESP32)

#define CHANNELS 1

#else

#error Unknown I2S arch

#endif

#else   // USE_I2S
#define CHANNELS 1
#endif  // USE_I2S

#define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_CONT | PDB_SC_PDBIE | PDB_SC_DMAEN)

class LS_DAC : CommandParser {
public:
  void Setup() {
    if (!needs_setup_) return;
    needs_setup_ = false;
#ifdef TEENSYDUINO
    dma.begin(true); // Allocate the DMA channel first

#ifdef USE_I2S

#if defined(ESP32)
    //#include "driver/i2s_std.h"
    //#include "driver/gpio.h"

    i2s_chan_handle_t tx_handle;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, &tx_handle, NULL);

    i2s_std_config_t std_cfg = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
      .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
      .gpio_cfg = {
	.mclk = I2S_GPIO_UNUSED,
	.bclk = bclkPin,
	.ws = lrclkPin,
	.dout = txd0Pin,
	.din = I2S_GPIO_UNUSED,
	.invert_flags = {
	  .mclk_inv = false,
	  .bclk_inv = false,
	  .ws_inv = false,
	},
      },
    };
    i2s_channel_init_std_mode(tx_handle, &std_cfg);

    /* Before write data, start the tx channel first */
    //i2s_channel_enable(tx_handle);
    //i2s_channel_write(tx_handle, src_buf, bytes_to_write, bytes_written, ticks_to_wait);

    /* If the configurations of slot or clock need to be updated,
     * stop the channel first and then update it */
    // i2s_channel_disable(tx_handle);
    // std_cfg.slot_cfg.slot_mode = I2S_SLOT_MODE_MONO; // Default is stereo
    // i2s_channel_reconfig_std_slot(tx_handle, &std_cfg.slot_cfg);
    // std_cfg.clk_cfg.sample_rate_hz = 96000;
    // i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg);

    /* Have to stop the channel before deleting it */
    //i2s_channel_disable(tx_handle);
    /* If the handle is not needed any more, delete it to release the channel resources */
    // i2s_del_channel(tx_handle);
#endif


#if defined(KINETISK) || defined(KINETISL)
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

#elif defined(__IMXRT1062__)

    CCM_CCGR5 |= CCM_CCGR5_SAI1(CCM_CCGR_ON);
    int fs = AUDIO_RATE;
    // PLL between 27*24 = 648MHz und 54*24=1296MHz
    int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
    int n2 = 1 + (24000000 * 27) / (fs * 256 * n1);

    double C = ((double)fs * 256 * n1 * n2) / 24000000;
    int c0 = C;
    int c2 = 10000;
    int c1 = C * c2 - (c0 * c2);
    my_set_audioClock(c0, c1, c2, false);

    // clear SAI1_CLK register locations
    CCM_CSCMR1 = (CCM_CSCMR1 & ~(CCM_CSCMR1_SAI1_CLK_SEL_MASK))
      | CCM_CSCMR1_SAI1_CLK_SEL(2); // &0x03 // (0,1,2): PLL3PFD0, PLL5, PLL4
    CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
      | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
      | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f

    // Select MCLK
    IOMUXC_GPR_GPR1 = (IOMUXC_GPR_GPR1
                       & ~(IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL_MASK))
      | (IOMUXC_GPR_GPR1_SAI1_MCLK_DIR | IOMUXC_GPR_GPR1_SAI1_MCLK1_SEL(0));

#ifdef AUDIO_CONTROL_SGTL5000
    // Most I2C boards don't need the MCLK, but the audio shield does.
    CORE_PIN23_CONFIG = 3;  //1:MCLK
#endif
    
    CORE_PIN21_CONFIG = 3;  //1:RX_BCLK
    CORE_PIN20_CONFIG = 3;  //1:RX_SYNC

    int rsync = 0;
    int tsync = 1;

    I2S1_TMR = 0;
    //I2S1_TCSR = (1<<25); //Reset
    I2S1_TCR1 = I2S_TCR1_RFW(1);
    I2S1_TCR2 = I2S_TCR2_SYNC(tsync) | I2S_TCR2_BCP // sync=0; tx is async;
      | (I2S_TCR2_BCD | I2S_TCR2_DIV((1)) | I2S_TCR2_MSEL(1));
    I2S1_TCR3 = I2S_TCR3_TCE;
    I2S1_TCR4 = I2S_TCR4_FRSZ((2-1)) | I2S_TCR4_SYWD((32-1)) | I2S_TCR4_MF
      | I2S_TCR4_FSD | I2S_TCR4_FSE | I2S_TCR4_FSP;
    I2S1_TCR5 = I2S_TCR5_WNW((32-1)) | I2S_TCR5_W0W((32-1)) | I2S_TCR5_FBT((32-1));

    I2S1_RMR = 0;
    //I2S1_RCSR = (1<<25); //Reset
    I2S1_RCR1 = I2S_RCR1_RFW(1);
    I2S1_RCR2 = I2S_RCR2_SYNC(rsync) | I2S_RCR2_BCP  // sync=0; rx is async;
      | (I2S_RCR2_BCD | I2S_RCR2_DIV((1)) | I2S_RCR2_MSEL(1));
    I2S1_RCR3 = I2S_RCR3_RCE;
    I2S1_RCR4 = I2S_RCR4_FRSZ((2-1)) | I2S_RCR4_SYWD((32-1)) | I2S_RCR4_MF
      | I2S_RCR4_FSE | I2S_RCR4_FSP | I2S_RCR4_FSD;
    I2S1_RCR5 = I2S_RCR5_WNW((32-1)) | I2S_RCR5_W0W((32-1)) | I2S_RCR5_FBT((32-1));

    CORE_PIN7_CONFIG  = 3;  //1:TX_DATA0
#endif

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
    dma.triggerAtHardwareEvent(DMAMUX_SOURCE_I2S0_TX);
    dma.enable();

    I2S0_TCSR = I2S_TCSR_SR;
    I2S0_TCSR = I2S_TCSR_TE | I2S_TCSR_BCE | I2S_TCSR_FRDE;
#elif defined(__IMXRT1062__)
    dma.TCD->SADDR = dac_dma_buffer;
    dma.TCD->SOFF = 2;
    dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
    dma.TCD->NBYTES_MLNO = 2;
    dma.TCD->SLAST = -sizeof(dac_dma_buffer);
    dma.TCD->DOFF = 0;
    dma.TCD->CITER_ELINKNO = sizeof(dac_dma_buffer) / 2;
    dma.TCD->DLASTSGA = 0;
    dma.TCD->BITER_ELINKNO = sizeof(dac_dma_buffer) / 2;
    dma.TCD->CSR = DMA_TCD_CSR_INTHALF | DMA_TCD_CSR_INTMAJOR;
    dma.TCD->DADDR = (void *)((uint32_t)&I2S1_TDR0 + 2);
    dma.triggerAtHardwareEvent(DMAMUX_SOURCE_SAI1_TX);
    dma.enable();

    I2S1_RCSR |= I2S_RCSR_RE | I2S_RCSR_BCE;
    I2S1_TCSR = I2S_TCSR_TE | I2S_TCSR_BCE | I2S_TCSR_FRDE;
#endif

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
#if PROFFIEBOARD_VERSION == 3
    stm32l4_dma_create(&dma, DMA_CHANNEL_DMA2_CH1_SAI1_A, STM32L4_SAI_IRQ_PRIORITY);
#else
    stm32l4_dma_create(&dma, DMA_CHANNEL_DMA2_CH6_SAI1_A, STM32L4_SAI_IRQ_PRIORITY);
#endif
#if defined(ENABLE_I2S_OUT) || defined(ENABLE_SPDIF_OUT)
    stm32l4_dma_create(&dma2, DMA_CHANNEL_DMA2_CH2_SAI1_B, STM32L4_SAI_IRQ_PRIORITY);
    // stm32l4_dma_create(&dma2, DMA_CHANNEL_DMA2_CH7_SAI1_B, STM32L4_SAI_IRQ_PRIORITY);
#endif

#endif
  }

  void begin() {
    if (on_) return;
    on_ = true;
    Setup();

    memset(dac_dma_buffer, 0, sizeof(dac_dma_buffer));
#if defined(ENABLE_SPDIF_OUT) || defined(ENABLE_I2S_OUT)
    memset(dac_dma_buffer2, 0, sizeof(dac_dma_buffer2));
#endif
#ifdef FILTER_CUTOFF_FREQUENCY
    filter_.clear();
#endif

#ifndef TEENSYDUINO
    stm32l4_system_periph_enable(SYSTEM_PERIPH_SAI1);
    stm32l4_dma_enable(&dma, &isr, 0);
    SAI_Block_TypeDef *SAIx = SAI1_Block_A;
    SAIx->CR1 = SAI_xCR1_DS_2 | SAI_xCR1_MONO | SAI_xCR1_CKSTR
#ifdef ENABLE_SPDIF_OUT
     | (1 << SAI_xCR1_MCKDIV_Pos)
#endif
    ;
    SAIx->FRCR = (31 << SAI_xFRCR_FRL_Pos) | (15 << SAI_xFRCR_FSALL_Pos) | SAI_xFRCR_FSDEF | SAI_xFRCR_FSOFF;
    SAIx->SLOTR = SAI_xSLOTR_NBSLOT_0 | (0x0003 << SAI_xSLOTR_SLOTEN_Pos) | SAI_xSLOTR_SLOTSZ_0;

#ifdef ENABLE_I2S_OUT
    stm32l4_dma_enable(&dma2, &isr2, 0);
    SAI_Block_TypeDef *SAI2 = SAI1_Block_B;
    SAI2->CR1 = SAI_xCR1_DS_2 | SAI_xCR1_MONO | SAI_xCR1_CKSTR;
    SAI2->FRCR = (31 << SAI_xFRCR_FRL_Pos) | (15 << SAI_xFRCR_FSALL_Pos) | SAI_xFRCR_FSDEF | SAI_xFRCR_FSOFF;
    SAI2->SLOTR = SAI_xSLOTR_NBSLOT_0 | (0x0003 << SAI_xSLOTR_SLOTEN_Pos) | SAI_xSLOTR_SLOTSZ_0;
#endif

#ifdef ENABLE_SPDIF_OUT
    stm32l4_dma_enable(&dma2, &isr2, 0);
    SAI_Block_TypeDef *SAI2 = SAI1_Block_B;
    SAI2->CR1 = SAI_xCR1_PRTCFG_0;
#endif

    stm32l4_system_saiclk_configure(SYSTEM_SAICLK_11289600);
#ifdef ENABLE_SPDIF_OUT
    my_stm32l4_system_saiclk_configure_22579200();
#endif
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

#define SAIB_SCK g_SAI1Pins.sck
#define SAIB_FS g_SAI1Pins.fs
#define SAIB_SD g_SAI1Pins.sd
    
#if PROFFIEBOARD_VERSION < 3
#undef SAIB_FS
#define SAIB_FS GPIO_PIN_PA4_SAI1_FS_B
#endif    

#ifdef ENABLE_I2S_OUT
    extern const stm32l4_sai_pins_t g_SAI1Pins;
    // Neopixel data 3 pin is SCK
    stm32l4_gpio_pin_configure(SAIB_SCK, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));

    // Neopixel data 4 is FS, it is also possible to use GPIO_PIN_PB6_SAI1_FS_B, which is the power button pin.
    stm32l4_gpio_pin_configure(SAIB_FS, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));

    // aux/Button2 button pin is DATA
    stm32l4_gpio_pin_configure(SAIB_SD, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_dma_start(&dma2, (uint32_t)&SAI2->DR, (uint32_t)dac_dma_buffer2, AUDIO_BUFFER_SIZE * 2,
                      DMA_OPTION_EVENT_TRANSFER_DONE |
                      DMA_OPTION_EVENT_TRANSFER_HALF |
                      DMA_OPTION_MEMORY_TO_PERIPHERAL |
                      DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
                      DMA_OPTION_MEMORY_DATA_SIZE_16 |
                      DMA_OPTION_MEMORY_DATA_INCREMENT |
                      DMA_OPTION_PRIORITY_HIGH |
                      DMA_OPTION_CIRCULAR);
    SAI2->CR1 |= SAI_xCR1_DMAEN;
#endif

#ifdef ENABLE_SPDIF_OUT
    extern const stm32l4_sai_pins_t g_SAI1Pins;
    // aux button pin becomes S/PDIF out
    stm32l4_gpio_pin_configure(SAIB_SD, (GPIO_PUPD_NONE | GPIO_OSPEED_HIGH | GPIO_OTYPE_PUSHPULL | GPIO_MODE_ALTERNATE));
    stm32l4_dma_start(&dma2, (uint32_t)&SAI2->DR, (uint32_t)dac_dma_buffer2, AUDIO_BUFFER_SIZE * 2 * 2,
                      DMA_OPTION_EVENT_TRANSFER_DONE |
                      DMA_OPTION_EVENT_TRANSFER_HALF |
                      DMA_OPTION_MEMORY_TO_PERIPHERAL |
                      DMA_OPTION_PERIPHERAL_DATA_SIZE_32 |
                      DMA_OPTION_MEMORY_DATA_SIZE_32 |
                      DMA_OPTION_MEMORY_DATA_INCREMENT |
                      DMA_OPTION_PRIORITY_HIGH |
                      DMA_OPTION_CIRCULAR);
    SAI2->CR1 |= SAI_xCR1_DMAEN;
#endif

    noInterrupts();
    if (!(SAIx->CR1 & SAI_xCR1_SAIEN))
    {
      SAIx->CR2 = SAI_xCR2_FTH_1;
      SAIx->CR1 |= SAI_xCR1_SAIEN;
    }
#if defined(ENABLE_I2S_OUT) || defined(ENABLE_SPDIF_OUT)
    if (!(SAI2->CR1 & SAI_xCR1_SAIEN))
    {
      SAI2->CR2 = SAI_xCR2_FTH_1;
      SAI2->CR1 |= SAI_xCR1_SAIEN;
    }
#endif // I2S || SPDIF
    interrupts();
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

#if defined(ENABLE_I2S_OUT) || defined(ENABLE_SPDIF_OUT)
    SAI_Block_TypeDef *SAI2 = SAI1_Block_B;

    SAI2->CR1 &=~ SAI_xCR1_SAIEN;
    // Poll until actually off
    while (SAI2->CR1 & SAI_xCR1_SAIEN);
#endif

    stm32l4_system_saiclk_configure(SYSTEM_SAICLK_NONE);

    stm32l4_dma_disable(&dma);
#if defined(ENABLE_I2S_OUT) || defined(ENABLE_SPDIF_OUT)
    stm32l4_dma_disable(&dma2);
#endif
    stm32l4_system_periph_disable(SYSTEM_PERIPH_SAI1);
    extern const stm32l4_sai_pins_t g_SAIPins;
    stm32l4_gpio_pin_configure(g_SAIPins.sck, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
    stm32l4_gpio_pin_configure(g_SAIPins.fs, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
    stm32l4_gpio_pin_configure(g_SAIPins.sd, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));

#ifdef ENABLE_I2S_OUT
    stm32l4_gpio_pin_configure(GPIO_PIN_PB3_SAI1_SCK_B, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
    stm32l4_gpio_pin_configure(GPIO_PIN_PA4_SAI1_FS_B, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
    stm32l4_gpio_pin_configure(GPIO_PIN_PB5_SAI1_SD_B, (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
#endif

#ifdef ENABLE_SPDIF_OUT
    stm32l4_gpio_pin_configure(GPIO_PIN_PB5_SAI1_SD_B,  (GPIO_PUPD_NONE | GPIO_MODE_ANALOG));
#endif

#endif
  }

  bool Parse(const char* cmd, const char* arg) override {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS

#ifdef FILTER_CUTOFF_FREQUENCY
    if (!strcmp(cmd, "filterdata")) {
      for (size_t i = 0; i < NELEM(filter_.data_); i++) {
	STDOUT << "filter[" << i << "] = " << filter_.data_[i][0] << ", " << filter_.data_[i][1] << "\n";
      }
      return true;
    }
#endif
    
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
#if defined(TEENSYDUINO) && !defined(USE_I2S)
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

  // TODO: Replace with enable/disable
  void SetStream(class ProffieOSAudioStream* stream) {
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
    int16_t *dest;
    uint32_t saddr = current_position();
#ifdef ENABLE_SPDIF_OUT
    uint32_t *secondary;
#endif
#ifdef ENABLE_I2S_OUT
    uint16_t *secondary;
#endif

#ifdef TEENSYDUINO
    dma.clearInterrupt();
#endif
    if (saddr < (uint32_t)dac_dma_buffer + sizeof(dac_dma_buffer) / 2) {
      // DMA is transmitting the first half of the buffer
      // so we must fill the second half
      dest = (int16_t *)&dac_dma_buffer[AUDIO_BUFFER_SIZE*CHANNELS];
#ifdef ENABLE_SPDIF_OUT
      secondary = dac_dma_buffer2 + AUDIO_BUFFER_SIZE * 2;
#endif
#ifdef ENABLE_I2S_OUT
      secondary = dac_dma_buffer2 + AUDIO_BUFFER_SIZE * CHANNELS;
#endif
    } else {
      // DMA is transmitting the second half of the buffer
      // so we must fill the first half
      dest = (int16_t *)dac_dma_buffer;
#if defined(ENABLE_SPDIF_OUT) || defined(ENABLE_I2S_OUT)
      secondary = dac_dma_buffer2;
#endif
    }
#ifdef __IMXRT1062__
    int16_t *clear_cache = dest;
#endif

#ifndef LINE_OUT_VOLUME
// LINE_OUT_VOLUME can be defined as dynamic_mixer.get_volume() to follow normal volume.
#define LINE_OUT_VOLUME 2000
#endif      
    
#if defined(ENABLE_SPDIF_OUT) || defined(ENABLE_I2S_OUT) || defined(FILTER_CUTOFF_FREQUENCY)
#define DAC_GET_FLOATS
#endif

#ifdef DAC_GET_FLOATS
    float data[AUDIO_BUFFER_SIZE];
#else    
    int16_t data[AUDIO_BUFFER_SIZE];
#endif    
    int n = 0;
    if (stream_) {
      n = dynamic_mixer.read(data, AUDIO_BUFFER_SIZE);
    }
    while (n < AUDIO_BUFFER_SIZE) data[n++] = 0;

#if defined(ENABLE_SPDIF_OUT) || defined(ENABLE_I2S_OUT)
    for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
#ifdef ENABLE_I2S_OUT
      *(secondary++) = clamptoi16(data[i] * (LINE_OUT_VOLUME));
#endif
#ifdef ENABLE_SPDIF_OUT
      int32_t sample24 = clamptoi24(data[i] * (LINE_OUT_VOLUME * 256));
      *(secondary++) = sample24 & 0xFFFFFF;
      *(secondary++) = sample24 & 0xFFFFFF;
#endif
    }
#endif  // GET_FLOATS

#ifdef FILTER_CUTOFF_FREQUENCY
    // Run the filter
    static_assert(AUDIO_BUFFER_SIZE % 4 == 0);
    for (int i = 0; i < AUDIO_BUFFER_SIZE; i+=4) {
      filter_.Run4(data + i);
    }
#endif    
    for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
#ifdef DAC_GET_FLOATS
      int16_t sample = clamptoi16(data[i] * dynamic_mixer.get_volume());
#else   // GET_FLOATS
      int16_t sample = data[i];
#endif  // GET_FLOATS

#ifdef USE_I2S
      *(dest++) = sample;
#if CHANNELS == 2
      // Duplicate sample to left and right channel.
      *(dest++) = sample;
#endif
#else // I2S
      // For Teensy DAC
      *(dest++) = (((uint16_t)sample) + 32768) >> 4;
#endif
    }
    
#ifdef __IMXRT1062__
    arm_dcache_flush_delete(clear_cache, sizeof(dac_dma_buffer)/2);
#endif
  }

  static void isr2(void* arg, unsigned long int event) {}
#ifndef DMAMEM
#define DMAMEM
#endif

  bool on_ = false;
  bool needs_setup_ = true;
  DMAMEM static uint16_t dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];
#ifdef ENABLE_SPDIF_OUT
  DMAMEM static uint32_t dac_dma_buffer2[AUDIO_BUFFER_SIZE*2*2];
#endif
#ifdef ENABLE_I2S_OUT
  DMAMEM static uint16_t dac_dma_buffer2[AUDIO_BUFFER_SIZE*2*CHANNELS];
#endif
  static ProffieOSAudioStream * volatile stream_;
  static DMAChannel dma;
#if defined(ENABLE_I2S_OUT) || defined(ENABLE_SPDIF_OUT)
  static DMAChannel dma2;
#endif
#ifdef FILTER_CUTOFF_FREQUENCY
  static Filter::Biquad<
    Filter::Bilinear<
    Filter::BLT<
      Filter::ConvertToHighPass<
	Filter::ButterWorthProtoType<FILTER_ORDER>, FILTER_CUTOFF_FREQUENCY, AUDIO_RATE>>>> filter_;
#endif  
};

#ifdef TEENSYDUINO
DMAChannel LS_DAC::dma(false);
#else
DMAChannel LS_DAC::dma;
#if defined(ENABLE_I2S_OUT) || defined(ENABLE_SPDIF_OUT)
DMAChannel LS_DAC::dma2;
#endif

#endif
ProffieOSAudioStream * volatile LS_DAC::stream_ = nullptr;
DMAMEM __attribute__((aligned(32))) uint16_t LS_DAC::dac_dma_buffer[AUDIO_BUFFER_SIZE*2*CHANNELS];
#ifdef ENABLE_SPDIF_OUT
DMAMEM __attribute__((aligned(32))) uint32_t LS_DAC::dac_dma_buffer2[AUDIO_BUFFER_SIZE*2*2];
#endif
#ifdef ENABLE_I2S_OUT
DMAMEM __attribute__((aligned(32))) uint16_t LS_DAC::dac_dma_buffer2[AUDIO_BUFFER_SIZE*2*CHANNELS];
#endif
#ifdef FILTER_CUTOFF_FREQUENCY
Filter::Biquad<
  Filter::Bilinear<
  Filter::BLT<
    Filter::ConvertToHighPass<
      Filter::ButterWorthProtoType<FILTER_ORDER>, FILTER_CUTOFF_FREQUENCY, AUDIO_RATE>>>> LS_DAC::filter_;
#endif  

LS_DAC dac;

#endif  // ESP32
#endif  // SOUND_DAC
