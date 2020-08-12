#ifndef SOUND_BEEPER_H
#define SOUND_BEEPER_H

// Beeper class, used for warning beeps and such.
class Beeper : public AudioStream {
public:
  int read(int16_t *data, int elements) override {
    SCOPED_PROFILER();
    int e = elements;
    while (num_beeps_) {
      int s = std::min(elements, beeps_[0].samples_);
      if (x_ == 0) {
        x_ = beeps_[0].f_;
	if (x_ == 0) {
	  x_ = beeps_[0].samples_;
	  value_ = 0;
	} else {
	  if (value_ <= 0) {
	    value_ = 200;
	  } else {
	    value_ = -200;
	  }
	}
      }
      s = std::min<int>(s, (int)x_);
      if (s <= 0) return e - elements;
      for (int i = 0; i < s; i++) data[i] = value_;
      data += s;
      elements -= s;
      x_ -= s;
      beeps_[0].samples_ -= s;
      if (beeps_[0].samples_ == 0) {
	num_beeps_--;
	for (size_t i = 0; i < num_beeps_; i++) beeps_[i] = beeps_[i + 1];
      }
    }
    return e - elements;
  }

  void Beep(float length, float freq) {
    EnableAmplifier();
    noInterrupts();
    if (num_beeps_ < NELEM(beeps_)) {
      beeps_[num_beeps_].f_ = freq == 0.0 ? 0 : AUDIO_RATE / freq / 2.0;
      beeps_[num_beeps_].samples_ = AUDIO_RATE * length;
      num_beeps_++;
    }
    interrupts();
  }
  void Silence(float length) {
    Beep(length, 0.0);
  }

  bool isPlaying() {
    return num_beeps_ > 0;
  }

  bool eof() const override {
    return !num_beeps_;
  }

private:
  struct Beep {
    int f_ = 0;
    int samples_ = 0;
  };
  struct Beep beeps_[10];
  volatile size_t num_beeps_ = 0;
  volatile int x_ = 0;
  volatile int value_ = 0;
};

#endif
