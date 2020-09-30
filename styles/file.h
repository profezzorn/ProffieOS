#ifndef STYLES_FILE_H
#define STYLES_FILE_H

#ifdef ENABLE_AUDIO

#include "../sound/audio_stream_work.h"

template<bool USE_HUM>
class FromFileStyleBase : private AudioStreamWork {
protected:
  struct Frame {
    volatile uint32_t frame;
    volatile char data[512]; // SD blocks are 512 bytes!
  };
  Frame& CurrentFrame() { return frames_[current_]; }
  Frame& BackFrame() { return frames_[1-current_]; }
  
  RefPtr<BufferedWavPlayer>& getPlayer() {
    if (USE_HUM) {
      return hybrid_font.hum_player_;
    } else {
      return track_player_;
    }
  }
  virtual uint32_t FrameNum() = 0;

public:
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
    if (next_available_) {
      uint32_t frame = FrameNum();
      if (frame >= BackFrame().frame || frame < CurrentFrame().frame) {
	noInterrupts();
	current_ = 1 - current_;
	next_available_ = false;
	interrupts();
	scheduleFillBuffer();
      }
    }
  }
  size_t space_available() const override {
    if (millis() - last_open_ < 500) return 0;
    return next_available_ ? 0 : 1;
  }
  bool FillBuffer() override {
    if (!file_.IsOpen()) {
      uint32_t now = millis();
      if (!now) now++;
      if (now - last_open_ < 500) return false;
      last_open_ = now;
      if (!getPlayer()) return false;
      if (!getPlayer()->isPlaying()) return false;
      if (!*getPlayer()->filename()) return false;
      strcpy(filename, getPlayer()->filename());
      int x = strlen(filename);
      if (x > 4) strcpy(filename + x - 3, "blc");
      file_.Open(filename);
      // Yield to make sure we don't upset the audio.
      return false;
    }
    if (!next_available_) {
      uint32_t frame = FrameNum();
      if (frame == CurrentFrame().frame) frame++;
      file_.Seek(frame * sizeof(BackFrame().data));
      file_.Read((uint8_t*)BackFrame().data, sizeof(BackFrame().data));
      BackFrame().frame = frame;
      next_available_ = true;
    }
    return true;
  }
  void CloseFiles() override {
    file_.Close();
  }
protected:
  // Approximate sRGB -> linear calculation
  uint16_t sqr(uint8_t x) { return x * x; }

  static char filename[128];
  static Frame frames_[2];
  static volatile int current_;
  static volatile bool next_available_;
  static volatile uint32_t last_open_;
  static FileReader file_;
  int num_leds_;
};

// Note, unused variables go away automatically...
template<bool USE_HUM> char FromFileStyleBase<USE_HUM>::filename[128];
template<bool USE_HUM> typename FromFileStyleBase<USE_HUM>::Frame FromFileStyleBase<USE_HUM>::frames_[2];
template<bool USE_HUM> volatile int FromFileStyleBase<USE_HUM>::current_;
template<bool USE_HUM> volatile bool FromFileStyleBase<USE_HUM>::next_available_;
template<bool USE_HUM> volatile uint32_t FromFileStyleBase<USE_HUM>::last_open_;
template<bool USE_HUM> FileReader FromFileStyleBase<USE_HUM>::file_;

template<int N = 170, int OFFSET=0, int FRAME_RATE_ENUMERATOR=30, int FRAME_RATE_DENOMINATOR=1>
class FromFileStyle : public FromFileStyleBase<false> {
public:
  uint32_t FrameNum() override {
    return floor(getPlayer()->pos() * FRAME_RATE_ENUMERATOR / FRAME_RATE_DENOMINATOR);
  }
  SimpleColor getColor(int led) {
    led = led * N / num_leds_ + OFFSET;
    return SimpleColor(Color16(sqr(CurrentFrame().data[led*3]),
			       sqr(CurrentFrame().data[led*3+1]),
			       sqr(CurrentFrame().data[led*3+2])));
  }
};

template<int N = 170, int OFFSET=0, int FRAME_RATE_ENUMERATOR=30, int FRAME_RATE_DENOMINATOR=1>
class FromHumFileStyle : public FromFileStyleBase<true> {
public:
  uint32_t FrameNum() override {
    return floor(getPlayer()->pos() * FRAME_RATE_ENUMERATOR / FRAME_RATE_DENOMINATOR);
  }
  SimpleColor getColor(int led) {
    led = led * N / num_leds_ + OFFSET;
    return SimpleColor(Color16(sqr(CurrentFrame().data[led*3]),
			       sqr(CurrentFrame().data[led*3+1]),
			       sqr(CurrentFrame().data[led*3+2])));
  }
};

#endif  // ENABLE_AUDIO
#endif
