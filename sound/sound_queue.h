#ifndef SOUND_SOUND_QUEUE_H
#define SOUND_SOUND_QUEUE_H

struct SoundToPlay {
  const char* filename_;
  Effect* effect_;
  int selection_;

  SoundToPlay() :filename_(nullptr), effect_(nullptr), selection_(-1) {}
  explicit SoundToPlay(const char* file) : filename_(file) {  }
  SoundToPlay(Effect* effect, int selection = -1) : filename_(nullptr), effect_(effect), selection_(selection) {}
  SoundToPlay(uint8_t R, uint8_t G, uint8_t B) :filename_(nullptr), effect_(nullptr), selection_((R << 16) + (G << 8) + B) {}
  bool Play(BufferedWavPlayer* player) {
    if (filename_) {
      return player->PlayInCurrentDir(filename_);
    }
    if (effect_) {
      effect_->Select(selection_);
      player->PlayOnce(effect_);
      return true;
    }
    // color
    char filename[32];
    strcpy(filename, "colors/");
    char* tmp = filename + strlen(filename);
    int N = selection_;
    for (int j = 0; j < 6; j++) {
      *(tmp++) = "0123456789abcdef"[(N >> 20) & 0xf];
      N <<= 4;
    }
    strcpy(tmp, ".wav");
    return player->PlayInCurrentDir(filename);
  }
   bool isSet() {
     return filename_ != nullptr || effect_ != nullptr || selection_ != -1;
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
    if (fadeout_) {
      fadeout_ = false;
      if (busy_) {
	// We really should add the time since the fadeout was scheduled here.
	// However, if polling is frequent, it would be a fraction of a milli
	// which basically dones't matter.
	player->set_fade_time(fadeout_len_);
	player->FadeAndStop();
      }
    }
    if (!busy_) {
      if (sounds_) {
	busy_ = true;
	if (!player) {
	  player = GetFreeWavPlayer();
	  if (!player) return;
	}
	player->set_volume_now(1.0f);
	queue_[0].Play(player.get());
	sounds_--;
	for (int i = 0; i < sounds_; i++) queue_[i] = queue_[i+1];
      } else {
	if (player) player.Free();
      }
    }
  }
  bool busy() const { return busy_; }
  void fadeout(float len) {
    sounds_ = 0;
    fadeout_ = true;
    fadeout_len_ = len;
  }
private:
  int sounds_;
  bool busy_ = false;
  bool fadeout_;
  bool fadeout_len_;
  SoundToPlay queue_[QueueLength];
};

#endif // SOUND_SOUND_QUEUE_H
