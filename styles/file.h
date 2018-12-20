#ifndef STYLES_FILE_H
#define STYLES_FILE_H

#include "audio/audio_stream_work.h"

class FromFileStyle : private AudioStreamWork {
public:
  Frame& CurrentFrame() { return frames_[current_]; }
  Frame& BackFrame() { return frames_[1-current_]; }
  
  void run(BladeBase* blade) {
    uint32_t now = millis();
    if (next_available_) {
      uint32_t frame = (millis() - start_) * frame_rate_ / 1000;
      if (frame >= BackFrame().frame) {
	NoInterrupts();
	current_ = 1 - current_;
	next_available_ = false;
	Interrupts();
	scheduleFillBuffer();
      }
    }
  }
  size_t space_available() const override {
    return next_available_ ? 0 : 1;
  }
  bool FillBuffer() override {
    if (!next_available_) {
      uint32_t frame = (millis() - start_) * frame_rate_ / 1000;
      if (frame_ == CurrentFrame().frame) frame++;
      file_.Seek(frame_ * 512);
      file_.Read(BackFrame().data, sizeof(BackFrame().data));
      BackFrame().frame = frame;
      next_available_ = true;
    }
  }
  OverDriveColor getColor(int led) {
    led += offset_;
    return Color16(Color8(CurrentFrame().data[led*3],
			  CurrentFrame().data[led*3+1],
			  CurrentFrame().data[led*3+2]));
  }
private:
  struct Frame {
    volatile uint32_t frame;
    volatile char data[512];
  };
  Frame frames_[2];
  volatile int current_;
  volatile bool next_available_;
  FileReader file_;
  uint32_t frame_rate;
};


#endif
