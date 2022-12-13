#ifndef SOUND_BEEPER_H
#define SOUND_BEEPER_H

#include "../common/circular_buffer.h"

// Beeper class, used for warning beeps and such.
class Beeper : public ProffieOSAudioStream {
public:
  Beeper() : ProffieOSAudioStream(), x_(0), value_(0) {}

  int read(int16_t *data, int elements) override {
    SCOPED_PROFILER();
    int e = elements;
    while (beeps_.size()) {
      int s = std::min(elements, beeps_.current().samples_);
      if (x_.get() == 0) {
        x_.set(beeps_.current().f_);
	if (x_.get() == 0) {
	  x_.set(beeps_.current().samples_);
	  value_.set(0);
	} else {
	  if (value_.get() <= 0) {
	    value_.set(200);
	  } else {
	    value_.set(-200);
	  }
	}
      }
      s = std::min<int>(s, (int)x_.get());
      if (s <= 0) return e - elements;
      for (int i = 0; i < s; i++) data[i] = value_.get();
      data += s;
      elements -= s;
      x_ -= s;
      beeps_.current().samples_ -= s;
      if (beeps_.current().samples_ == 0) {
	beeps_.pop();
      }
    }
    return e - elements;
  }

  void Beep(float length, float freq) {
    EnableAmplifier();
    if (beeps_.space_available()) {
      beeps_.next().f_ = freq == 0.0 ? 0 : AUDIO_RATE / freq / 2.0;
      beeps_.next().samples_ = AUDIO_RATE * length;
      beeps_.push();
    }
  }
  void Silence(float length) {
    Beep(length, 0.0);
  }

  bool isPlaying() {
    return beeps_.size() > 0;
  }

  bool eof() const override {
    return beeps_.size() == 0;
  }

private:
  struct Beep {
    int f_ = 0;
    int samples_ = 0;
  };
  CircularBuffer<struct Beep, 16> beeps_;
  POAtomic<int> x_;
  POAtomic<int> value_;
};

#endif
