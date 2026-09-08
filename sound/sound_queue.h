#ifndef SOUND_SOUND_QUEUE_H
#define SOUND_SOUND_QUEUE_H

#ifdef ENABLE_AUDIO

class SoundToPlayBase {
public:
  virtual bool Play(BufferedWavPlayer* player) = 0;
  virtual ~SoundToPlayBase() {}
};

class SoundToPlayInSameDirAs : public SoundToPlayBase {
public:
  SoundToPlayInSameDirAs(const char* filename, Effect* e) : filename_(filename), effect_(e) {}
  bool Play(BufferedWavPlayer* player) override {
    player->PlayInSameDirAs(filename_, effect_);
    return true;
  }

private:
  const char* filename_;
  Effect* effect_;
};

class SoundToPlayInCurrentDir : public SoundToPlayBase {
public:
  SoundToPlayInCurrentDir(const char* filename) : filename_(filename) {}
  bool Play(BufferedWavPlayer* player) override {
    return player->PlayInCurrentDir(filename_);
  }

private:
  const char* filename_;
};

// Like SoundToPlayInCurrentDir but also searches the "errors" directory
// as a fallback.  Used by PlayErrorMessage()
class SoundToPlayErrorFile : public SoundToPlayBase {
public:
  // |dir| is either a string literal, or a pointer into current_directory.
  SoundToPlayErrorFile(const char* dir, const char* filename)
    : dir_(dir), filename_(filename) {}
  bool Play(BufferedWavPlayer* player) override {
    // PlayErrorMessage() already found the file, so play it directly
    // instead of searching all the font dirs again.
    return player->PlayInDir(dir_, filename_);
  }
private:
  const char* dir_;
  const char* filename_;
};

class SoundToPlayFileID : public SoundToPlayBase {
public:
  SoundToPlayFileID(Effect::FileID id) : file_id_(id) {}
  bool Play(BufferedWavPlayer* player) override {
    player->PlayOnce(file_id_);
    return true;
  }

private:
  Effect::FileID file_id_;
};

class SoundToPlayColor : public SoundToPlayBase {
public:
  SoundToPlayColor(unsigned char r,
		   unsigned char g,
		   unsigned char b) : r_(r), g_(g), b_(b) {
  }
  bool Play(BufferedWavPlayer* player) override {
    // color
    char filename[32];
    strcpy(filename, "colors/");
    char* tmp = filename + strlen(filename);
    *(tmp++) = "0123456789abcdef"[r_ >> 4];
    *(tmp++) = "0123456789abcdef"[r_ & 15];
    *(tmp++) = "0123456789abcdef"[g_ >> 4];
    *(tmp++) = "0123456789abcdef"[g_ & 15];
    *(tmp++) = "0123456789abcdef"[b_ >> 4];
    *(tmp++) = "0123456789abcdef"[b_ & 15];
    strcpy(tmp, ".wav");
    return player->PlayInCurrentDir(filename);
  }
  unsigned char r_, g_, b_;
};

// Fixed-size deque of objects with shared base class.
// Similar to deque<shared_ptr<BASE>>, size is fixed and
// all memory allocations are done from internal memory.
// push_back/emplace_back will fail and return false/nullptr
// if insufficient space is available.
template<class BASE, size_t N = 8, size_t SIZE = N * 12>
class VirtVec {
public:
  size_t size() const { return elements_.size(); }
  BASE* operator[](size_t n) { return elements_[n]; }
  BASE* first() { return elements_[0]; }

  template<class T, class... Args>
  T* emplace_back(Args&&... args) {
    if (elements_.space_available() == 0) return nullptr;
    char* ptr = allocate(sizeof(T), alignof(T));
    if (ptr == nullptr) return nullptr;
    T* ret = new(ptr) T(args...);
    elements_.push_back(ret);
    return ret;
  }

  template<class T> bool push_back(T value) {
    if (elements_.space_available() == 0) return false;
    char* ptr = allocate(sizeof(T), alignof(T));
    if (ptr == nullptr) return false;
    elements_.push_back(new(ptr) T(value));
    return true;
  }

  // Stack
  void pop_back() {
    if (elements_.empty()) return;
    int n = elements_.size() -1;
    (*this)[n]->~BASE();
    buffer_.unpush(element_size(n));
    elements_.unpush(-1);
  }

  // Queue
  void pop_front() {
    if (elements_.empty()) return;
    (*this)[0]->~BASE();
    buffer_.pop(element_size(0));
    elements_.pop();
  }

  void clear() {
    while (size()) pop_front();
  }

private:
  // May return a larger number, but not smaller.
  size_t element_size(size_t n) {
    char *a = reinterpret_cast<char *>(elements_[n]);
    char *b;
    if (n == size() - 1) {
      // Last element
      b = buffer_.space();
    } else {
      b = reinterpret_cast<char *>(elements_[n + 1]);
    }
    if (b < a) b += SIZE;
    return b - a;
  }

  char* allocate(size_t size, size_t alignment) {
    while (true) {
      if (buffer_.space_available() < size) return nullptr;
      if (((size_t)buffer_.space()) % alignment) {
	// Fix alignment
	size_t misalignment = alignment - ((size_t)buffer_.space()) % alignment;
	if (buffer_.space_available() < misalignment) return nullptr;
	buffer_.push(misalignment);
	continue; // retry
      }
      if (buffer_.continuous_space() < size) {
	// Not enough space at end of buffer,
	// jump back to beginning of buffer.
	// (If there is enough space there.)
	if (buffer_.space_available() - buffer_.continuous_space() < size) return nullptr;
	buffer_.push(buffer_.continuous_space());
	continue; // retry
      }
      break;
    }
    char* ret = buffer_.space();
    buffer_.push(size);
    return ret;
  }

  // Pointers to each of the elements in the VirtVec
  CircularBuffer<BASE*, N> elements_;

  // Memory for storing the elements.
  CircularBuffer<char, SIZE> buffer_;
};

struct SoundToPlay {
  const char* filename_;
  Effect::FileID file_id_;

  SoundToPlay() :filename_(nullptr), file_id_(nullptr, 0xffff, 0, 0)  {}
  explicit SoundToPlay(const char* file) : filename_(file) {  }
  explicit SoundToPlay(const char* file, Effect* effect) : filename_(file), file_id_(effect, 0,0,0) {  }
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

class SoundToPlayAfterDelay : public SoundToPlayBase {
public:
  SoundToPlayAfterDelay(Effect* effect) : sound_(effect) {}
  bool Play(BufferedWavPlayer* player) override {
    if (delay_timer().Active()) return false;
    return sound_.Play(player);
  }

private:
  SoundToPlay sound_;
};

template<int QueueLength>
class SoundQueue {
public:
  template<class T>
  bool Play(T p) { return queue_.push_back(p); }

  // For backwards compatibility only.
  bool Play(SoundToPlay p) {
    if (p.filename_) {
      return Play(SoundToPlayInCurrentDir(p.filename_));
    }
    if (p.file_id_) {
      return Play(SoundToPlayFileID(p.file_id_));
    }
    return Play(SoundToPlayColor(p.file_id_.GetFileNum(),
				 p.file_id_.GetSubId(),
				 p.file_id_.GetAlt()));
  }

  bool Play(const char* p) { return Play(SoundToPlayInCurrentDir(p)); }

  // Called from Loop()
  void PollSoundQueue(RefPtr<BufferedWavPlayer>& player) {
    busy_ = player && player->isPlaying();
    if (fadeout_) {
      fadeout_ = false;
      if (busy_) {
        // We really should add the time since the fadeout was scheduled here.
        // However, if polling is frequent, it would be a fraction of a milli
        // which basically doesn't matter.
        player->set_fade_time(fadeout_len_);
        player->FadeAndStop();
      }
    }
    if (!busy_) {
      if (queue_.size()) {
        if (delay_timer().Active()) return;
        if (!player) {
          player = GetFreeWavPlayer();
          if (!player) return;
        }
        player->set_volume_now(1.0f);
        if (queue_[0]->Play(player.get())) {
          busy_ = true;
          queue_.pop_front();
        }
      } else {
        if (player) player.Free();
      }
    }
    // Duck everything else while the queue is speaking, so that error
    // messages, menu prompts, etc. stay intelligible even if the prop has
    // other sounds going.  The hold is short and re-armed on every poll,
    // which means we don't need to know how long the sound is (asking the
    // player for length() before it has been opened doesn't work), and
    // sounds that start in the middle of the speech get ducked as well.
    if (busy_ && player) {
      DodgeSound(DODGE_HOLD_MILLIS);
      // ...but not the sound we are trying to make audible.
      player->set_dodge(false);
    }
  }
  bool busy() const { return busy_; }
  void fadeout(float len) {
    queue_.clear();
    fadeout_ = true;
    fadeout_len_ = len;
  }
  void clear_pending() {
    queue_.clear();
  }
private:
  // How long other sounds stay ducked after the last poll that found the
  // queue busy.  Re-armed on every poll, so this only decides how quickly
  // things come back up once the queue goes quiet.
  static const uint32_t DODGE_HOLD_MILLIS = 250;
  bool busy_ = false;
  bool fadeout_;
  bool fadeout_len_;
  VirtVec<SoundToPlayBase, QueueLength, QueueLength * 8> queue_;
};

#endif // ENABLE_AUDIO

#endif // SOUND_SOUND_QUEUE_H
