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
class BufferedAudioStream : public ProffieOSAudioStream, public AudioStreamWork {
public:
  BufferedAudioStream() : AudioStreamWork(),
			  stream_(0),
			  buf_start_(0),
			  buf_end_(0),
			  eof_(true) {
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
      to_copy = std::min(to_copy, bufsize);
      int start_pos = buf_start_.get() & (N-1);
      to_copy = std::min(to_copy, N - start_pos);
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
    return !buffered() && eof_.get();
  }
  void StopFromReader() override {
    if (stream_.get())
      stream_.get()->StopFromReader();
  }
  void clear() {
    eof_.set(true);
    buf_start_.set(buf_end_.get());
    stream_.set(NULL);
  }
  int buffered() const {
    return buf_end_.get() - buf_start_.get();
  }
  // Overridable
  size_t space_available() const override {
    return real_space_available();
  }
  void SetStream(ProffieOSAudioStream* stream) {
    eof_.set(false);
    stream_.set(stream);
  }
private:
  size_t real_space_available() const {
    if (eof_.get() || !stream_.get()) return 0;
    return N - buffered();
  }
  bool FillBuffer() override {
    if (stream_.get()) {
      size_t space = real_space_available();
      if (space) {
        size_t end_pos = buf_end_.get() & (N-1);
        size_t to_read = std::min(space, N - end_pos);
        int got = stream_.get()->read(buffer_ + end_pos, to_read);
        if (got) {
          eof_.set(false);
        } else if (stream_.get()) {
          eof_.set(stream_.get()->eof());
	} else {
	  eof_.set(true);
        }
        buf_end_ += got;
      }
    }
    return stream_.get() && space_available() > 0 && !eof_.get();
  }
  POAtomic<ProffieOSAudioStream*> stream_;
  // Note, these are assumed to be atomic, 8-bit processors won't work.
  POAtomic<size_t> buf_start_;
  POAtomic<size_t> buf_end_;
  POAtomic<bool> eof_;
  int16_t buffer_[N];
};

#endif
