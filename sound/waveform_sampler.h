#ifndef SOUND_WAVEFORM_SAMPLER_H
#define SOUND_WAVEFORM_SAMPLER_H

struct WaveForm {
  int16_t table_[1024];
};

struct WaveFormSampler {
  WaveFormSampler(const WaveForm& waveform) : waveform_(waveform.table_), pos_(0), delta_(0) {}
  WaveFormSampler(const int16_t* waveform) : waveform_(waveform), pos_(0), delta_(0) {}
  const int16_t *waveform_;
  int pos_;
  volatile int delta_;
  int16_t next() {
    pos_ += delta_;
    if (pos_ > 1024 * 65536) pos_ -= 1024 * 65536;
    // Bilinear lookup here?
    return waveform_[pos_ >> 16];
  }
};

#endif
