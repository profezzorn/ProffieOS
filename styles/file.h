#ifndef STYLES_FILE_H
#define STYLES_FILE_H

#ifdef ENABLE_AUDIO

#include "../sound/audio_stream_work.h"

template<int N = 170, int OFFSET=0, int FRAME_RATE_ENUMERATOR=30, int FRAME_RATE_DENOMINATOR=1>
class FromFileStyle : private AudioStreamWork {
private:
  struct Frame {
    volatile uint32_t frame;
    volatile char data[512]; // SD blocks are 512 bytes!
  };
  Frame& CurrentFrame() { return frames_[current_]; }
  Frame& BackFrame() { return frames_[1-current_]; }

public:
  uint32_t FrameNum() {
    return floor(track_player_->pos() * FRAME_RATE_ENUMERATOR / FRAME_RATE_DENOMINATOR);
  }
  
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
    if (next_available_) {
      uint32_t frame = FrameNum();
      if (frame >= BackFrame().frame) {
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
      if (!track_player_) return false;
      if (!track_player_->isPlaying()) return false;
      if (!*track_player_->filename()) return false;
      strcpy(filename, track_player_->filename());
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
  // Approximate sRGB -> linear calculation
  uint16_t sqr(uint8_t x) { return x * x; }
  OverDriveColor getColor(int led) {
    led = led * N / num_leds_ + OFFSET;
    OverDriveColor ret;
    ret.c = Color16(sqr(CurrentFrame().data[led*3]),
		    sqr(CurrentFrame().data[led*3+1]),
		    sqr(CurrentFrame().data[led*3+2]));
    ret.overdrive = false;
    return ret;
  }
private:
  char filename[128];
  Frame frames_[2];
  volatile int current_;
  volatile bool next_available_;
  volatile uint32_t last_open_;
  FileReader file_;
  int num_leds_;
};

#endif  // ENABLE_AUDIO
#endif
