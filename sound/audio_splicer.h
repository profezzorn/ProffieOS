#ifndef SOUND_AUDIO_SPLICER_H
#define SOUND_AUDIO_SPLICER_H

// This class is used to cut from one sound to another with
// no gap. It does a short (2.5ms) crossfade to accomplish this.
// It's currently hard-coded to use wav_players[0] and wav_playes[1].
class AudioSplicer : public AudioStream, Looper {
public:
  AudioSplicer() : Looper(NOLINK) {}
  const char* name() override { return "AudioSplicer"; }

  int read(int16_t* data, int elements) override {
    int16_t *p = data;
    int to_read = elements;
    if (current_ < 0 && fadeto_ < 0) {
      return 0;
    }
    if (current_ >= 0) {
      int num = wav_players[current_].read(p, to_read);
      to_read -= num;
      p += num;
      if (to_read > 0) {
        // end of file?
        if (wav_players[current_].eof()) {
          // STDOUT.println("AudioSPlicer::EOF!!");
          current_ = -1;
        }
      }
    }
    while (to_read > 0) {
      *(p++) = 0;
      to_read --;
    }
    if (fadeto_ >= 0) {
      p = data;
      to_read = elements;
      while (to_read) {
        int16_t tmp[32];
        int n = min(to_read, (int)NELEM(tmp));
        int num = wav_players[fadeto_].read(tmp, n);
        while (num < n) tmp[num++] = 0;
        for (int i = 0; i < num; i++) {
          p[i] = (p[i] * fade_ + tmp[i] * (32768 - fade_)) >> 15;
          if (fade_) {
            fade_ -= fade_speed_;
            if (fade_ < 0) fade_ = 0;
          }
        }
        to_read -= n;
        p += n;
      }
      if (!fade_) {
        if (current_ != -1)
          wav_players[current_].Stop();
        current_ = fadeto_;
        fadeto_ = -1;
      }
    }
    return elements;
  }

  bool eof() const override {
    return current_ == -1 && fadeto_ == -1;
  }

  void set_crossover_time(float t) {
    fade_speed_ = max(1, (int)(32768 / t / AUDIO_RATE));
#if 0
    STDOUT.print("FADE SPEED: ");
    STDOUT.println(fade_speed_);
#endif
  }

  Effect* next_effect_ = NULL;
  Effect* next_loop_ = NULL;

  void Loop() override {
    if (fadeto_ == -1) {
      Effect *f = next_effect_;
      Effect *l = next_loop_;
      next_effect_ = next_loop_ = NULL;
      if (f) Play(f, l);
      Looper::Unlink();
    }
  }
          
  bool Play(Effect* f, Effect* loop) {
    if (fadeto_ != -1) {
      if (!next_effect_) Looper::Link();
      next_effect_ = f;
      next_loop_ = loop;
      return false;
    }
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    int unit = current_ == 0 ? 1 : 0;
    wav_players[unit].PlayOnce(f);
    if (loop) {
      wav_players[unit].PlayLoop(loop);
    }
    if (current_ == -1) {
      current_ = unit;
    } else {
      fadeto_ = unit;
      fade_ = 32768;
    }
    return true;
  }

  bool isPlaying() const {
    return current_ != -1 || fadeto_ != -1;
  }

protected:
  volatile int current_= -1;
  volatile int fadeto_ = -1;
  volatile int fade_speed_ = 128;
  volatile int fade_;
};

#endif
