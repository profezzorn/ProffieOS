#ifndef SOUND_AUDIOSTREAM_H
#define SOUND_AUDIOSTREAM_H

#include <stdint.h>

class ProffieOSAudioStream {
public:
  virtual int read(int16_t* data, int elements) = 0;
  // There is no need to call eof() unless read() returns zero elements.
  virtual bool eof() const { return false; }
  // Cannot be called at the same time as read().
  virtual void StopFromReader() {}
};

#endif
