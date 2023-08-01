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
    pause_.set(true);
    clear();
    wav.Play(filename);
    SetStream(&wav);
    scheduleFillBuffer();
    pause_.set(false);
  }

  bool PlayInCurrentDir(const char* name) {
    STDOUT << "Playing " << name << ", ";
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
    STDOUT << "(not found)\n";
    return false;
  }

  void UpdateSaberBaseSoundInfo() {
    SaberBase::sound_length = length();
    SaberBase::sound_number = current_file_id().GetFileNum();
  }

  void PlayOnce(Effect::FileID fileid, float start = 0.0) {
    const Effect* effect = fileid.GetEffect();
    MountSDCard();
    EnableAmplifier();
    set_volume_now(volume_target() * effect->GetVolume() / 100);
    STDOUT << "unit = " << WhatUnit(this) << " vol = " << volume() << ", ";

    pause_.set(true);
    clear();
    ResetStopWhenZero();
    wav.PlayOnce(fileid, start);
    SetStream(&wav);
    // Fill up the buffer, if possible.
    while (!wav.eof() && space_available()) {
      scheduleFillBuffer();
    }
    pause_.set(false);
    if (SaberBase::sound_length == 0.0 && effect->GetFollowing() != effect) {
      UpdateSaberBaseSoundInfo();
    }
  }
  void PlayOnce(Effect* effect, float start = 0.0) {
    PlayOnce(effect->RandomFile(), start);
  }
  void PlayLoop(Effect* effect) { wav.PlayLoop(effect); }

  // Do not call from interrupts!
  void Stop() {
#if 0
    // Immediate stop is impossible for multithreaded implementations.
    pause_.set(true);
    wav.Stop();
#else
    int v = volume_target();
    set_fade_time(0.005);
    FadeAndStop();
    while (isPlaying()) delay(1);
    set_volume_now(v);
#endif    
    wav.Close();
    clear();
  }

  void CloseFiles() override { wav.Close(); }

  const char* Filename() const {
    return wav.Filename();
  }

  Effect::FileID current_file_id() const {
    return wav.current_file_id();
  }
  
  bool isPlaying() const {
    return !pause_.get() && (wav.isPlaying() || buffered());
  }

  BufferedWavPlayer() : pause_(true) {
    SetStream(&wav);
  }

  // This makes a paused player report very little available space, which
  // means that it will be low priority for reading.
  size_t space_available() const override {
    size_t ret = VolumeOverlay<BufferedAudioStream<AUDIO_BUFFER_SIZE_BYTES>>::space_available();
    if (pause_.get() && ret) ret = 2; // still slightly higher than FromFileStyle<>
    return ret;
  }

  int read(int16_t* dest, int to_read) override {
    if (pause_.get()) return 0;
    return VolumeOverlay<BufferedAudioStream<AUDIO_BUFFER_SIZE_BYTES> >::read(dest, to_read);
  }
  bool eof() const override {
    if (pause_.get()) return true;
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

  void dump() {
    STDOUT << " pause=" << pause_.get()
	   << " buffered=" << buffered()
	   << " wav.isPlaying()=" << wav.isPlaying()
	   << "\n";
    wav.dump();
  }
private:
  uint32_t refs_ = 0;

  PlayWav wav;
  POAtomic<bool> pause_;
};

#endif
