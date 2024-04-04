#ifndef SOUND_SOUND_QUEUE_H
#define SOUND_SOUND_QUEUE_H

struct SoundToPlay {
  const char* filename_;
  Effect* effect_;
  int selection_;

  SoundToPlay() :filename_(nullptr), effect_(nullptr) {}
  explicit SoundToPlay(const char* file) : filename_(file){  }
  SoundToPlay(Effect* effect, int selection = -1) : filename_(nullptr), effect_(effect), selection_(selection) {}
  bool Play(BufferedWavPlayer* player) {
     if (filename_) return player->PlayInCurrentDir(filename_);
     effect_->Select(selection_);
     player->PlayOnce(effect_);
     return true;
   }
   bool isSet() {
      return filename_ != nullptr || effect_ != nullptr;
   }
};

template<int QueueLength>
class SoundQueue {
public:
  bool Play(SoundToPlay p) {
    if (sounds_ < QueueLength) {
      queue_[sounds_++] = p;
      return true;
    }
    return false;
  }
  bool Play(const char* p) {
    return Play(SoundToPlay(p));
  }
  // Called from Loop()
  void PollSoundQueue(RefPtr<BufferedWavPlayer>& player) {
    busy_ = player && player->isPlaying();
    if (sounds_ &&  !busy_) {
      busy_ = true;
      if (!player) {
        player = GetFreeWavPlayer();
        if (!player) return;
	player->set_volume_now(1.0f);
      }
      queue_[0].Play(player.get());
      sounds_--;
      for (int i = 0; i < sounds_; i++) queue_[i] = queue_[i+1];
    }
  }
  bool busy() const { return busy_; }
private:
  int sounds_;
  bool busy_ = false;
  SoundToPlay queue_[QueueLength];
};

#endif // SOUND_SOUND_QUEUE_H
