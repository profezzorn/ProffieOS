#ifndef SOUND_BUFFERED_WAV_PLAYER_H
#define SOUND_BUFFERED_WAV_PLAYER_H

#include "playwav.h"
#include "volume_overlay.h"

// Combines a WavPlayer and a BufferedAudioStream into a
// buffered wav player. When we start a new sample, we
// make sure to fill up the buffer before we start playing it.
// This minimizes latency while making sure to avoid any gaps.
class BufferedWavPlayer : public VolumeOverlay<BufferedAudioStream<512> > {
public:
  void Play(const char* filename) {
    MountSDCard();
    EnableAmplifier();
    pause_ = true;
    clear();
    wav.Play(filename);
    SetStream(&wav);
    scheduleFillBuffer();
    pause_ = false;
  }

  void PlayOnce(Effect* effect, float start = 0.0) {
    MountSDCard();
    EnableAmplifier();
    STDOUT.print("unit = ");
    STDOUT.print(WhatUnit(this));
    STDOUT.print(" vol = ");
    STDOUT.print(volume());
    STDOUT.print(", ");

    pause_ = true;
    clear();
    ResetStopWhenZero();
    wav.PlayOnce(effect, start);
    SetStream(&wav);
    scheduleFillBuffer();
    pause_ = false;
  }
  void PlayLoop(Effect* effect) { wav.PlayLoop(effect); }

  void Stop() override {
    pause_ = true;
    wav.Stop();
    clear();
  }

  void Close() { wav.Close(); }

  const char* Filename() const {
    return wav.Filename();
  }
  
  bool isPlaying() const {
    return !pause_ && (wav.isPlaying() || buffered());
  }

  BufferedWavPlayer() {
    SetStream(&wav);
  }

  int read(int16_t* dest, int to_read) override {
    if (pause_) return 0;
    return VolumeOverlay<BufferedAudioStream<512> >::read(dest, to_read);
  }

  float length() const { return wav.length(); }

  void AddRef() { refs_++; }
  void SubRef() { refs_--; }
  bool Available() const { return refs_ == 0 && !isPlaying(); }
  uint32_t refs() const { return refs_; }
private:
  uint32_t refs_ = 0;

  PlayWav wav;
  volatile bool pause_;
};

#endif
