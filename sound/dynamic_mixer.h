#ifndef SOUND_DYNAMIC_MIXER_H
#define SOUND_DYNAMIC_MIXER_H

#include <algorithm>

// Audio compressor, takes N input channels, sums them and divides the
// result by the square root of the average volume.
template<int N> class AudioDynamicMixer : public AudioStream, Looper {
public:
  AudioDynamicMixer() {
    for (int i = 0; i < N; i++) {
      streams_[i] = nullptr;
    }
  }
  const char* name() override { return "AudioDynamicMixer"; }

#if defined(STM32L433xx) && defined(__FAST_MATH__)
  // Faster AND smaller
  int my_sqrt(int x) { return sqrtf(x); }
#else  
  // Calculate square root of |x|, using the previous square
  // root as a guess.
  int my_sqrt(int x) {
    if (x <= 0) return 0;
    int over, under, step = 1;
    if (last_square_ * last_square_ > x) {
      over = last_square_;
      under = over - 1;
      while (under * under > x) {
        over = under;
        under -= step;
        step += step;
        if (under <= 0) { under = 0; break; }
      }
    } else {
      under = last_square_;
      over = under + 1;
      while (over * over <= x) {
        under = over;
        over += step;
        step += step;
        if (over < 0) { over = x; break; }
      }
    }
    while (under + 1 < over) {
      int mid = (over + under) >> 1;
      if (mid * mid > x) {
        over = mid;
      } else {
        under = mid;
      }
    }
    return last_square_ = under;
  }
  int last_square_ = 0;
#endif
  
  int read(int16_t* data, int elements) override {
    SCOPED_PROFILER();
    int32_t sum[AUDIO_BUFFER_SIZE];
    int ret = elements;
    int v = 0, v2 = 0;
    num_samples_ += elements;
    while (elements) {
      int to_do = std::min(elements, (int)NELEM(sum));
      for (int i = 0; i < to_do; i++) sum[i] = 0;
      for (int i = 0; i < N; i++) {
	if (!streams_[i]) continue;
        int e = streams_[i]->read(data, to_do);
	if (e < to_do && !streams_[i]->eof()) {
	  underflow_count_++;
	}
        for (int j = 0; j < e; j++) {
          sum[j] += data[j];
        }
      }

      for (int i = 0; i < to_do; i++) {
        v = sum[i];
        vol_ = ((vol_ + abs(v)) * 255) >> 8;
        v2 = v * volume_ / (my_sqrt(vol_) + 100);
//	v2 = (int)((v * (float)volume_)/(sqrtf(vol_)+100.0f));
        data[i] = clamptoi16(v2);
        peak_sum_ = std::max<int32_t>(abs(v), peak_sum_);
        peak_ = std::max<int32_t>(abs(v2), peak_);
      }
      data += to_do;
      elements -= to_do;
    }
    last_sample_ = v2;
    last_sum_ = v;
    
//    STDOUT.println(vol_);
    return ret;
  }

  // No volume, no clamping!
  int read(float* data, int elements) {
    SCOPED_PROFILER();
    int32_t sum[AUDIO_BUFFER_SIZE];
    int16_t tmp[AUDIO_BUFFER_SIZE];
    int ret = elements;
    int v = 0, v2 = 0;
    num_samples_ += elements;
    while (elements) {
      int to_do = std::min(elements, (int)NELEM(sum));
      for (int i = 0; i < to_do; i++) sum[i] = 0;
      for (int i = 0; i < N; i++) {
	if (!streams_[i]) continue;
        int e = streams_[i]->read(tmp, to_do);
	if (e < to_do && !streams_[i]->eof()) {
	  underflow_count_++;
	}
        for (int j = 0; j < e; j++) {
          sum[j] += tmp[j];
        }
      }

      for (int i = 0; i < to_do; i++) {
        v = sum[i];
        vol_ = ((vol_ + abs(v)) * 255) >> 8;
	data[i] = v / (sqrtf(vol_) + 100.0f);
      }
      data += to_do;
      elements -= to_do;
    }
    last_sample_ = v2 * volume_;
    last_sum_ = v;
    
//    STDOUT.println(vol_);
    return ret;
  }

  void Loop() override {
    uint32_t underflows = underflow_count_;
    if (underflows != last_underflow_count_) {
      if (millis() - last_printout_ > 100) {
	uint32_t new_underflows = underflows - last_underflow_count_;
	STDOUT.print("Audio underflows: ");
	STDOUT.println(new_underflows);
	last_underflow_count_ = underflows;
	last_printout_ = millis();
      }
    }
    if (monitor.ShouldPrint(Monitoring::MonitorSamples)) {
      STDOUT.print("Samples: ");
      STDOUT.print(num_samples_);
      STDOUT.print(" AVG volume: ");
      STDOUT.print(vol_);
      STDOUT.print(" last input sample: ");
      STDOUT.print(last_sum_);
      STDOUT.print(" last output sample:");
      STDOUT.print(last_sample_);
      STDOUT.print(" peak sum: ");
      STDOUT.print(peak_sum_);
      STDOUT.print(" peak: ");
      STDOUT.print(peak_);
      STDOUT.print(" underflows: ");
      STDOUT.println(underflow_count_);
      peak_sum_ = peak_ = 0;
    }
  }

  // TODO: Make levels monitorable

  int32_t last_sample() const {
    return last_sample_;
  }

  int32_t last_sum() const {
    return last_sum_;
  }

  int32_t audio_volume() const {
    return vol_;
  }

  void set_volume(int32_t volume) { volume_ = volume; }
  int32_t get_volume() const { return volume_; }

  AudioStream* streams_[N];
  int32_t vol_ = 0;
  int32_t last_sample_ = 0;
  int32_t last_sum_ = 0;
  int32_t peak_sum_ = 0;
  int32_t peak_ = 0;
  int32_t num_samples_ = 0;
  int32_t volume_ = VOLUME;
  volatile uint32_t underflow_count_ = 0;
  uint32_t last_underflow_count_ = 0;
  uint32_t last_printout_ = 0;
//  int32_t sum_;
//  ClickAvoiderLin volume_;
};

AudioDynamicMixer<NUM_WAV_PLAYERS + 2> dynamic_mixer;

#endif
