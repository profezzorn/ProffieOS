#ifndef FUNCTIONS_VOLUME_LEVEL_H
#define FUNCTIONS_VOLUME_LEVEL_H

#include "svf.h"

// Usage: VolumeLevel
// Returns 0-32768 based on volume level.
// returned value: INTEGER

class VolumeLevelSVF {
 public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    return dynamic_mixer.get_volume() * 32768 / VOLUME;
  }
};

using VolumeLevel = SingleValueAdapter<VolumeLevelSVF>;

#endif
