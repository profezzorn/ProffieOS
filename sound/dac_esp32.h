#ifndef SOUND_ESP32_DAC_H
#define SOUND_ESP32_DAC_H

#include <driver/i2s.h>
#include "esp32-hal.h"

void ls_dac_isr(void);

class LS_DAC : CommandParser {
public:
  void Setup() {
    if (!needs_setup_) return;
    needs_setup_ = false;

    timer_ = timerBegin(0, 2, true);
    timerAttachInterrupt(timer_, &ls_dac_isr, true);
    timerAlarmWrite(timer_, getApbFrequency() / 2 / 1500, true);
  }

  void begin() {
    if (on_) return;
    on_ = true;
    Setup();
    i2s_config_t config = {};
    config.mode             = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    config.sample_rate          = 44100;
    config.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    config.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
    config.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1; // interrupt priority
    config.dma_buf_count        = 2;
    config.dma_buf_len          = AUDIO_BUFFER_SIZE;
    config.use_apll             = false;
    config.tx_desc_auto_clear   = true;
    config.fixed_mclk           = I2S_PIN_NO_CHANGE;
    config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S);

    i2s_driver_install(I2S_NUM_0, &config, 0, NULL);
    
    i2s_pin_config_t pins = {};
    pins.mck_io_num = I2S_PIN_NO_CHANGE;
    pins.bck_io_num = bclkPin;
    pins.ws_io_num = lrclkPin;
    pins.data_out_num = txd0Pin;
    pins.data_in_num = I2S_PIN_NO_CHANGE;
    
    i2s_set_pin(I2S_NUM_0, &pins);
    i2s_zero_dma_buffer(I2S_NUM_0);

#ifdef FILTER_CUTOFF_FREQUENCY
    filter_.clear();
#endif

    pos_ = AUDIO_BUFFER_SIZE;
    timerAlarmEnable(timer_);
  }

  void end() {
    if (!on_) return;
    on_ = false;
    timerAlarmDisable(timer_);
    i2s_driver_uninstall(I2S_NUM_0);
  }

  bool Parse(const char* cmd, const char* arg) override {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "dacbuf")) {
      STDOUT << "Current position: " << pos_ << "\n";
      for (size_t i = 0; i < NELEM(dac_dma_buffer); i++) {
        STDOUT.print(dac_dma_buffer[i]);
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

  // Interrupt handler.
  // Fills the dma buffer with new sample data.
  void isr(void) {
    while (true) {
      if (pos_ < AUDIO_BUFFER_SIZE) {
	size_t written = 0;
	i2s_write(I2S_NUM_0,
		  dac_dma_buffer + pos_,
		  sizeof(dac_dma_buffer) - pos_ * sizeof(dac_dma_buffer),
		  &written,
		  0);
	pos_ += written >> 1;
	if (pos_ < sizeof(dac_dma_buffer)) return;
      }
      int n = 0;
#if defined(FILTER_CUTOFF_FREQUENCY)
      float data[AUDIO_BUFFER_SIZE];
      if (stream_) {
	n = dynamic_mixer.read(data, AUDIO_BUFFER_SIZE);
      }
      while (n < AUDIO_BUFFER_SIZE) data[n++] = 0;
      // Run the filter
      static_assert(AUDIO_BUFFER_SIZE % 4 == 0);
      for (int i = 0; i < AUDIO_BUFFER_SIZE; i+=4) {
	filter_.Run4(data + i);
      }
      for (int i = 0; i < AUDIO_BUFFER_SIZE;i++) {
	uint16_t sample = clamptoi16(data[i] * dynamic_mixer.get_volume());
	dac_dma_buffer[i*2] = sample;
	dac_dma_buffer[i*2+1] = sample;
      }
#else
      int16_t data[AUDIO_BUFFER_SIZE];
      if (stream_) {
	n = dynamic_mixer.read(data, AUDIO_BUFFER_SIZE);
      }
      while (n < AUDIO_BUFFER_SIZE) data[n++] = 0;
      for (int i = 0; i < AUDIO_BUFFER_SIZE;i++) {
	int16_t sample = data[i];
	dac_dma_buffer[i*2] = sample;
	dac_dma_buffer[i*2+1] = sample;
      }
#endif
      pos_ = 0;
    }
  }

private:
  size_t pos_;
  hw_timer_t *timer_;
  bool on_ = false;
  bool needs_setup_ = true;
  int16_t dac_dma_buffer[AUDIO_BUFFER_SIZE * 2];
  static ProffieOSAudioStream * volatile stream_;
#ifdef FILTER_CUTOFF_FREQUENCY
  static Filter::Biquad<
    Filter::Bilinear<
    Filter::BLT<
      Filter::ConvertToHighPass<
	Filter::ButterWorthProtoType<FILTER_ORDER>, FILTER_CUTOFF_FREQUENCY, AUDIO_RATE>>>> filter_;
#endif  
};

ProffieOSAudioStream * volatile LS_DAC::stream_ = nullptr;
LS_DAC dac;

void ls_dac_isr(void) {
  ScopedCycleCounter cc(audio_dma_interrupt_cycles);
  dac.isr();
}

#endif  // SOUND_ESP32_DAC_H
