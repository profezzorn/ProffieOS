#ifndef STYLES_FILE_H
#define STYLES_FILE_H

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
    uint32_t now = millis();
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
    if (open_fail_) return 0;
    return next_available_ ? 0 : 1;
  }
  bool FillBuffer() override {
    if (!file_.IsOpen()) {
      if (open_fail_) return;
      if (!track_player_) return;
      if (!*track_player_->filename()) return;
      strcpy(filename, track_player_->filename());
      int x = strlen(filename);
      if (x > 4) strcpy(filename + x - 3, "blc");
      if (!file_.Open(filename)) {
	open_fail_ = true;
	// TODO(hubbe): Re-try open later
      }
      // Yield to make sure we don't upset the audio.
      return;
    }
    if (!next_available_) {
      uint32_t frame = FrameNum();
      if (frame == CurrentFrame().frame) frame++;
      file_.Seek(frame * sizeof(BackFrame().data));
      file_.Read(BackFrame().data, sizeof(BackFrame().data));
      BackFrame().frame = frame;
      next_available_ = true;
    }
  }
  // Approximate sRGB -> linear calculation
  uint16_t sqr(uint8_t x) { return x * x; }
  OverDriveColor getColor(int led) {
    led = led * N / num_leds_ + OFFSET;
    return Color16(sqr(CurrentFrame().data[led*3]),
		   sqr(CurrentFrame().data[led*3+1]),
		   sqr(CurrentFrame().data[led*3+2]));
  }
private:
  char filename[128];
  Frame frames_[2];
  volatile int current_;
  volatile bool next_available_;
  volatile bool open_fail_;
  FileReader file_;
  int num_leds_;
};


#endif
