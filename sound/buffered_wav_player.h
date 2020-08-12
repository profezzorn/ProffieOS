#ifndef SOUND_BUFFERED_WAV_PLAYER_H
#define SOUND_BUFFERED_WAV_PLAYER_H

#include "playwav.h"
#include "volume_overlay.h"

class BufferedWavPlayer;
size_t WhatUnit(class BufferedWavPlayer* player);

#ifndef AUDIO_BUFFER_SIZE_BYTES
#define AUDIO_BUFFER_SIZE_BYTES 512
#endif

// Combines a WavPlayer and a BufferedAudioStream into a
// buffered wav player. When we start a new sample, we
// make sure to fill up the buffer before we start playing it.
// This minimizes latency while making sure to avoid any gaps.
class BufferedWavPlayer : public VolumeOverlay<BufferedAudioStream<AUDIO_BUFFER_SIZE_BYTES> > {
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

  bool PlayInCurrentDir(const char* name) {
    for (const char* dir = current_directory; dir; dir = next_current_directory(dir)) {
      PathHelper full_name(dir, name);
      LOCK_SD(true);
      bool exists = LSFS::Exists(full_name);
      LOCK_SD(false);
      // Fill up audio buffers before we lock the SD again
      AudioStreamWork::scheduleFillBuffer();
      if (exists) {
	Play(full_name);
	return true;
      }
    }
    return false;
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
    // Fill up the buffer, if possible.
    while (!wav.eof() && space_available()) {
      scheduleFillBuffer();
    }
    pause_ = false;
  }
  void PlayLoop(Effect* effect) { wav.PlayLoop(effect); }

  void Stop() override {
    pause_ = true;
    wav.Stop();
    clear();
  }

  void CloseFiles() override { wav.Close(); }

  const char* Filename() const {
    return wav.Filename();
  }
  
  bool isPlaying() const {
    return !pause_ && (wav.isPlaying() || buffered());
  }

  BufferedWavPlayer() {
    SetStream(&wav);
  }

  // This makes a paused player report very little available space, which
  // means that it will be low priority for reading.
  size_t space_available() const override {
    size_t ret = VolumeOverlay<BufferedAudioStream<AUDIO_BUFFER_SIZE_BYTES>>::space_available();
    if (pause_ && ret) ret = 2; // still slightly higher than FromFileStyle<>
    return ret;
  }

  int read(int16_t* dest, int to_read) override {
    if (pause_) return 0;
    return VolumeOverlay<BufferedAudioStream<AUDIO_BUFFER_SIZE_BYTES> >::read(dest, to_read);
  }
  bool eof() const override {
    if (pause_) return true;
    return VolumeOverlay<BufferedAudioStream<AUDIO_BUFFER_SIZE_BYTES> >::eof();
  }

  float length() const { return wav.length(); }
  float pos() const {
    return wav.pos() - buffered() * (1.0f / AUDIO_RATE);
  }
  const char* filename() const {
    return wav.filename();
  }

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
