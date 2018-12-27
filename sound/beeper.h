#ifndef SOUND_BEEPER_H
#define SOUND_BEEPER_H

// Beeper class, used for warning beeps and such.
class Beeper : public AudioStream {
public:
  int read(int16_t *data, int elements) override {
    int e = elements;
    while (true) {
      int s = min(elements, samples_);
      s = min(s, x_);
      if (s <= 0) return e - elements;
      if (up_) {
        for (int i = 0; i < s; i++) data[i] = 200;
      } else {
        for (int i = 0; i < s; i++) data[i] = -200;
      }
      data += s;
      elements -= s;
      x_ -= s;
      samples_ -= s;
      if (x_ == 0) {
        x_ = f_;
        up_ = !up_;
      }
    }
  }

  void Beep(float length, float freq) {
    EnableAmplifier();
    x_ = f_ = AUDIO_RATE / freq / 2.0;
    samples_ = AUDIO_RATE * length;
  }

  bool isPlaying() {
    return samples_ > 0;
  }

private:  
  volatile int samples_ = 0;
  volatile int f_ = 0;
  volatile int x_ = 0;
  volatile bool up_ = false;
};

#endif
