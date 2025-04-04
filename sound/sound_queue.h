#ifndef SOUND_SOUND_QUEUE_H
#define SOUND_SOUND_QUEUE_H

struct SoundToPlay {
  const char* filename_;
  Effect::FileID file_id_;

  SoundToPlay() :filename_(nullptr), file_id_(nullptr, 0xffff, 0, 0)  {}
  explicit SoundToPlay(const char* file) : filename_(file) {  }
  SoundToPlay(Effect* effect) : filename_(nullptr), file_id_(effect->RandomFile()) {}
  SoundToPlay(Effect* effect, int selection) : filename_(nullptr), file_id_((effect->Select(selection),effect->RandomFile())) {}
  SoundToPlay(uint8_t R, uint8_t G, uint8_t B) :filename_(nullptr), file_id_(nullptr, R, G, B) {}
  bool Play(BufferedWavPlayer* player) {
    if (filename_) {
      return player->PlayInCurrentDir(filename_);
    }
    if (file_id_) {
      player->PlayOnce(file_id_);
      return true;
    }
    if (file_id_.GetFileNum() > 0xff) return false;
    // color
    char filename[32];
    strcpy(filename, "colors/");
    char* tmp = filename + strlen(filename);
    *(tmp++) = "0123456789abcdef"[file_id_.GetFileNum() >> 4];
    *(tmp++) = "0123456789abcdef"[file_id_.GetFileNum() & 15];
    *(tmp++) = "0123456789abcdef"[file_id_.GetSubId()  >> 4];
    *(tmp++) = "0123456789abcdef"[file_id_.GetSubId() & 15];
    *(tmp++) = "0123456789abcdef"[file_id_.GetAlt() >> 4];
    *(tmp++) = "0123456789abcdef"[file_id_.GetAlt() & 15];
    strcpy(tmp, ".wav");
    return player->PlayInCurrentDir(filename);
  }
   bool isSet() {
     return filename_ != nullptr || file_id_ || file_id_.GetFileNum() != 0xffff;
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
  void clear_pending() {
    sounds_ = 0;
  }
private:
  int sounds_;
  bool busy_ = false;
  bool fadeout_;
  bool fadeout_len_;
  SoundToPlay queue_[QueueLength];
};

#endif // SOUND_SOUND_QUEUE_H
