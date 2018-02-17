#ifndef SOUND_AUDIOSTREAM_H
#define SOUND_AUDIOSTREAM_H

#include <stdint.h>

class AudioStream {
public:
  virtual int read(int16_t* data, int elements) = 0;
  // There is no need to call eof() unless read() returns zero elements.
  virtual bool eof() const { return false; }
  // Stop
  virtual void Stop() {}
};

#endif
