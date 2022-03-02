#ifndef FUNCTIONS_SOUND_LEVEL_H
#define FUNCTIONS_SOUND_LEVEL_H
#ifdef ENABLE_AUDIO

// Usage: SmoothSoundLevel
// Returns 0-32768 based on sound level.
// returned value: INTEGER
class SmoothSoundLevelSVF {
 public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    return clampi32(sqrtf(dynamic_mixer.audio_volume()) * 20, 0, 32768);
  }
};

using SmoothSoundLevel = SingleValueAdapter<SmoothSoundLevelSVF>;

// Usage: NoisySoundLevel
// Returns 0-32768 based on sound level.
// returned value: INTEGER
class NoisySoundLevelSVF {
 public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    return clampi32(dynamic_mixer.last_sum() * 3, 0, 32768);
  }
};

using NoisySoundLevel = SingleValueAdapter<NoisySoundLevelSVF>;

// Usage: NoisySoundLevelCompat
// Returns 0-32768 based on sound level.
// This function is now used to implement the
// AudioFlicker<> style, don't change it.
// returned value: INTEGER
class NoisySoundLevelCompatSVF {
 public:
  void run(BladeBase* blade) {}
  int calculate(BladeBase* blade) {
    return clampi32(dynamic_mixer.last_sum() << 3, 0, 32768);
  }
};

using NoisySoundLevelCompat = SingleValueAdapter<NoisySoundLevelCompatSVF>;

#else  // ENABLE_AUDIO

using NoisySoundLevel = Int<0>;
using NoisySoundLevelCompat = Int<0>;

#endif  // ENABLE_AUDIO
#endif
