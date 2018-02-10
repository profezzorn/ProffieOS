#ifndef SOUND_BUFFERED_AUDIO_STREAM_H
#define SOUND_BUFFERED_AUDIO_STREAM_H

#include "audio_stream_work.h"

// BufferedAudioStream is meant to be read from the main autdio interrupt.
// Every time some space is freed up, Schedulefillbuffer() is called
// to handle filling up the buffer at a lower interrupt level. Since
// filling up the buffer can mean reading from SD, there can potentially
// be more reads from the buffer while we're working on filling it up.
// To make this work, we need make sure that the end pointer for the buffer
// is only modified in the FillBuffer() function and the begin pointer is
// only modified in read();
// N needs to be power of 2
template<int N>
class BufferedAudioStream : public AudioStream, public AudioStreamWork {
public:
  BufferedAudioStream() : AudioStreamWork() {
  }
  int read(int16_t* buf, int bufsize) override {
#if 0
    // Disable buffer
    return stream_ ? stream_->read(buf, bufsize) : 0;
#else
    int copied = 0;
    while (bufsize) {
      int to_copy = buffered();
      if (!to_copy) break;
      to_copy = min(to_copy, bufsize);
      int start_pos = buf_start_ & (N-1);
      to_copy = min(to_copy, N - start_pos);
      memcpy(buf, buffer_ + start_pos, sizeof(buf[0]) * to_copy);
      copied += to_copy;
      buf_start_ += to_copy;
      buf += to_copy;
      bufsize -= to_copy;
    }
    scheduleFillBuffer();
    return copied;
#endif
  }
  bool eof() const override {
    return !buffered() && eof_;
  }
  void Stop() override { if (!stream_) stream_->Stop(); }
  void clear() {
    eof_ = false;
    buf_start_ = buf_end_;
    stream_ = NULL;
  }
  int buffered() const {
    return buf_end_ - buf_start_;
  }
  size_t space_available() const override {
    if (eof_ || !stream_) return 0;
    return N - buffered();
  }
  void SetStream(AudioStream* stream) {
    eof_ = false;
    stream_ = stream;
  }
private:
  bool FillBuffer() override {
    if (stream_) {
      size_t space = space_available();
      if (space) {
        size_t end_pos = buf_end_ & (N-1);
        size_t to_read = min(space, N - end_pos);
        int got = stream_->read(buffer_ + end_pos, to_read);
        if (got) {
          eof_ = false;
        } else {
          eof_ = stream_->eof();
        }
        buf_end_ += got;
      }
    }
    return stream_ && space_available() > 0 && !eof_;
  }
  AudioStream* volatile stream_ = 0;
  // Note, these are assumed to be atomic, 8-bit processors won't work.
  volatile size_t buf_start_ = 0;
  volatile size_t buf_end_ = 0;
  volatile bool eof_ = false;
  int16_t buffer_[N];
};

#endif
