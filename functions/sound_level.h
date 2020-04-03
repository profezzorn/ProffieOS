#ifndef FUNCTIONS_SOUND_LEVEL_H
#define FUNCTIONS_SOUND_LEVEL_H
#ifdef ENABLE_AUDIO

// Usage: SmoothSoundLevel
// Returns 0-32768 based on sound level.
// returned value: INTEGER
class SmoothSoundLevel {
 public:
  void run(BladeBase* blade) {
    value_ = clampi32(sqrtf(dynamic_mixer.audio_volume()) * 20, 0, 32768);
  }
  int getInteger(int led) { return value_; }
  int value_;
};

// Usage: NoisySoundLevel
// Returns 0-32768 based on sound level.
// returned value: INTEGER
class NoisySoundLevel {
 public:
  void run(BladeBase* blade) {
    value_ = clampi32(dynamic_mixer.last_sum() * 3, 0, 32768);
  }
  int getInteger(int led) { return value_; }
  int value_;
};


// Usage: NoisySoundLevelCompat
// Returns 0-32768 based on sound level.
// This function is now used to implement the
// AudioFlicker<> style, don't change it.
// returned value: INTEGER
class NoisySoundLevelCompat {
 public:
  void run(BladeBase* blade) {
    value_ = clampi32(dynamic_mixer.last_sum() << 3, 0, 32768);
  }
  int getInteger(int led) { return value_; }
  int value_;
};

#endif  // ENABLE_AUDIO
#endif
