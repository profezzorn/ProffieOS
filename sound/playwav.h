#ifndef SOUND_PLAYWAV_H
#define SOUND_PLAYWAV_H

// Simple upsampler code, doubles the number of samples with
// 2-lobe lanczos upsampling.
#define C1 24757
#define C2 -8191

#if 1
#define UPSAMPLE_FUNC(NAME, EMIT)                               \
  void NAME(int16_t sample) {                                   \
    upsample_buf_##NAME##_a_ = upsample_buf_##NAME##_b_;        \
    upsample_buf_##NAME##_b_ = upsample_buf_##NAME##_c_;        \
    upsample_buf_##NAME##_c_ = upsample_buf_##NAME##_d_;        \
    upsample_buf_##NAME##_d_ = sample;                          \
    EMIT(clamptoi16((upsample_buf_##NAME##_a_ * C2 +            \
          upsample_buf_##NAME##_b_ * C1 +                       \
          upsample_buf_##NAME##_c_ * C1 +                       \
                   upsample_buf_##NAME##_d_ * C2) >> 15));      \
    EMIT(upsample_buf_##NAME##_c_);                             \
  }                                                             \
  void clear_##NAME() {                                         \
    upsample_buf_##NAME##_a_ = 0;                               \
    upsample_buf_##NAME##_b_ = 0;                               \
    upsample_buf_##NAME##_c_ = 0;                               \
    upsample_buf_##NAME##_d_ = 0;                               \
  }                                                             \
  int16_t upsample_buf_##NAME##_a_ = 0;                         \
  int16_t upsample_buf_##NAME##_b_ = 0;                         \
  int16_t upsample_buf_##NAME##_c_ = 0;                         \
  int16_t upsample_buf_##NAME##_d_ = 0
#else
#define UPSAMPLE_FUNC(NAME, EMIT)               \
  void NAME(int16_t sample) {                   \
      EMIT(sample);      EMIT(sample);          \
  }                                             \
  void clear_##NAME() {                         \
  }
#endif

#define DOWNSAMPLE_FUNC(NAME, EMIT)                     \
  void NAME(int16_t sample) {                           \
    if (downsample_flag_##NAME##_) {                    \
      EMIT((downsample_buf_##NAME##_ + sample) >> 1);   \
      downsample_flag_##NAME##_ = false;                \
    } else {                                            \
      downsample_buf_##NAME##_ = sample;                \
      downsample_flag_##NAME##_ = true;                 \
    }                                                   \
  }                                                     \
  void clear_##NAME() {                                 \
    downsample_buf_##NAME##_ = 0;                       \
    downsample_flag_##NAME##_ = false;                  \
  }                                                     \
  int16_t downsample_buf_##NAME##_ = 0;                 \
  bool downsample_flag_##NAME##_ = false

// PlayWav reads a file from serialflash or SD and converts
// it into a stream of samples. Note that because it can
// spend some time reading data between samples, the
// reader must have enough buffers to provide smooth playback.
class PlayWav : StateMachine, public AudioStream {
public:
  void Play(const char* filename) {
    if (!*filename) return;
    strcpy(filename_, filename);
    run_ = true;
  }

  const char* Filename() const {
    return filename_;
  }

  void PlayOnce(Effect* effect, float start = 0.0) {
    if (effect->Play(filename_)) {
      start_ = start;
      effect_ = nullptr;
      run_ = true;
    }
  }
  void PlayLoop(Effect* effect) {
    effect_ = effect;
  }

  void Stop() override {
    state_machine_.reset_state_machine();
    effect_ = nullptr;
    run_ = false;
    written_ = num_samples_ = 0;
  }

  bool isPlaying() const {
    return run_;
  }

private:
  void Emit1(uint16_t sample) {
    samples_[num_samples_++] = sample;
  }
  UPSAMPLE_FUNC(Emit2, Emit1);
  UPSAMPLE_FUNC(Emit4, Emit2);
  DOWNSAMPLE_FUNC(Emit05, Emit1);

  uint32_t header(int n) const {
    return ((uint32_t *)buffer)[n+2];
  }

  template<int bits> int16_t read2() {
    if (bits == 8) return *(ptr_++) << 8;
    ptr_ += bits / 8 - 2;
    return *((*((int16_t**)&ptr_))++);
  }

  template<int bits, int channels, int rate>
  void DecodeBytes4() {
    while (ptr_ < end_ - channels * bits / 8 &&
           num_samples_ < (int)NELEM(samples_)) {
      int v = 0;
      if (channels == 1) {
        v = read2<bits>();
      } else {
        v = read2<bits>();
        v += read2<bits>();
        v >>= 1;
      }
      if (rate == AUDIO_RATE) {
        Emit1(v);
      } else if (rate == AUDIO_RATE / 2) {
        Emit2(v);
      } else if (rate == AUDIO_RATE / 4) {
        Emit4(v);
      } else if (rate == AUDIO_RATE * 2) {
        Emit05(v);
      } else {
        STDOUT.println("Unsupported rate.");
        Stop();
      }
    }
  }

  template<int bits, int channels>
  void DecodeBytes3() {
    if (rate_ == 44100)
      DecodeBytes4<bits, channels, 44100>();
    else if (rate_ == 22050)
      DecodeBytes4<bits, channels, 22050>();
    else if (rate_ == 11025)
      DecodeBytes4<bits, channels, 11025>();
  }

  template<int bits>
  void DecodeBytes2() {
    if (channels_ == 1) DecodeBytes3<bits, 1>();
    else DecodeBytes3<bits, 2>();
  }

  void DecodeBytes() {
    if (bits_ == 8) DecodeBytes2<8>();
    else if (bits_ == 16) DecodeBytes2<16>();
    else if (bits_ == 24) DecodeBytes2<24>();
    else if (bits_ == 32) DecodeBytes2<32>();
  }

  int ReadFile(int n) {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) {
      return sf_file_.read(buffer + 8, n);
    }
#endif
#ifdef ENABLE_SD
    return sd_file_.read(buffer + 8, n);
#else
    return 0;
#endif
  }

  void Skip(int n) {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) {
      sf_file_.seek(sf_file_.position() + n);
      return;
    }
#endif
#ifdef ENABLE_SD
    sd_file_.seek(sd_file_.position() + n);
#endif
  }

  void Rewind() {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) {
      sf_file_.seek(0);
      return;
    }
#endif
#ifdef ENABLE_SD
    sd_file_.seek(0);
#endif
  }

  uint32_t Tell() {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) return sf_file_.position();
#endif
#ifdef ENABLE_SD
    return sd_file_.position();
#endif
    return 0;
  }

  uint32_t FileSize() {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) return sf_file_.size();
#endif
#ifdef ENABLE_SD
    return sd_file_.size();
#endif
    return 0;
  }

  int AlignRead(int n) {
#ifdef ENABLE_SERIALFLASH
    if (sf_file_) return n;
#endif
#ifdef ENABLE_SD
    int next_block = (sd_file_.position() + 512u) & ~511u;
    int bytes_to_end_of_block = next_block - sd_file_.position();
    return min(n, bytes_to_end_of_block);
#else
    return n;
#endif
  }

  void loop() {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (!run_ && !effect_) YIELD();
      new_file_id_ = Effect::FileID();
      if (!run_) {
        new_file_id_ = effect_->RandomFile();
        if (!new_file_id_) goto fail;
        new_file_id_.GetName(filename_);
        run_ = true;
      }
      if (new_file_id_ && new_file_id_ == old_file_id_) {
        // Minor optimization: If we're reading the same file
        // as before, then seek to 0 instead of open/close file.
        Rewind();
      } else {
#ifdef ENABLE_SERIALFLASH
        sf_file_ = SerialFlashChip::open(filename_);
        if (!sf_file_)
#endif
        {
#ifdef ENABLE_SD
          sd_file_.close();
          sd_file_ = LSFS::Open(filename_);
          YIELD();
          if (!sd_file_)
#endif
          {
            STDOUT.print("File ");
            STDOUT.print(filename_);
            STDOUT.println(" not found.");
            goto fail;
          }
        }
        old_file_id_ = new_file_id_;
      }
      wav_ = endswith(".wav", filename_);
      if (wav_) {
        if (ReadFile(12) != 12) {
          STDOUT.println("Failed to read 12 bytes.");
          goto fail;
        }
        if (header(0) != 0x46464952 || header(2) != 0x45564157) {
          STDOUT.println("Not RIFF WAVE.");
          YIELD();
          goto fail;
        }

        // Look for FMT header.
        while (true) {
          if (ReadFile(8) != 8) {
            STDOUT.println("Failed to read 8 bytes.");
            goto fail;
          }

          len_ = header(1);
          if (header(0) != 0x20746D66) {  // 'fmt '
            Skip(len_);
            continue;
          }
          if (len_ < 16) {
            STDOUT.println("FMT header is wrong size..");
            goto fail;
          }
          break;
        }
        
        if (16 != ReadFile(16)) {
          STDOUT.println("Read failed.");
          goto fail;
        }
        if (len_ > 16) Skip(len_ - 16);
        if ((header(0) & 0xffff) != 1) {
          STDOUT.println("Wrong format.");
          goto fail;
        }
        channels_ = header(0) >> 16;
        rate_ = header(1);
        bits_ = header(3) >> 16;
      } else {
         channels_ = 1;
         rate_ = 44100;
         bits_ = 16;
      }
      STDOUT.print("channels: ");
      STDOUT.print(channels_);
      STDOUT.print(" rate: ");
      STDOUT.print(rate_);
      STDOUT.print(" bits: ");
      STDOUT.println(bits_);

      ptr_ = buffer + 8;
      end_ = buffer + 8;
      
      while (true) {
        if (wav_) {
          if (ReadFile(8) != 8) break;
          len_ = header(1);
          if (header(0) != 0x61746164) {
            Skip(len_);
            continue;
          }
        } else {
          if (Tell() >= FileSize()) break;
          len_ = FileSize() - Tell();
        }
        sample_bytes_ = len_;

        if (start_ != 0.0) {
          int samples = fmod(start_, length()) * rate_;
          int bytes_to_skip = samples * channels_ * bits_ / 8;
          Skip(bytes_to_skip);
          len_ -= bytes_to_skip;
          start_ = 0.0;
        }

        while (len_) {
          {
            int bytes_read = ReadFile(AlignRead(min(len_, 512u)));
            if (bytes_read == 0)
              break;
            len_ -= bytes_read;
            end_ = buffer + 8 + bytes_read;
          }
          while (ptr_ < end_ - channels_ * bits_ / 8) {
            DecodeBytes();

            while (written_ < num_samples_) {
              // Preload should go to here...
              while (to_read_ == 0) YIELD();

              int n = min(num_samples_ - written_, to_read_);
              memcpy(dest_, samples_ + written_, n * 2);
              dest_ += n;
              written_ += n;
              to_read_ -= n;
            }
            written_ = num_samples_ = 0;
          }
          if (ptr_ < end_) {
            memmove(buffer + 8 - (end_ - ptr_),
                    ptr_,
                    end_ - ptr_);
          }
          ptr_ = buffer + 8 - (end_ - ptr_);
        }
        YIELD();
      }

      // EOF;
      run_ = false;
      continue;

  fail:
      run_ = false;
      YIELD();
    }

    STATE_MACHINE_END();
  }

public:
  // Called from interrupt handler.
  int read(int16_t* dest, int to_read) override {
    dest_ = dest;
    to_read_ = to_read;
    loop();
    return dest_ - dest;
  }

  bool eof() const override {
    return !run_;
  }

  // Length, seconds.
  float length() const {
    return (float)(sample_bytes_) * 8 / (bits_ * rate_);
  }

private:
  volatile bool run_ = false;
  Effect* volatile effect_ = nullptr;
  Effect::FileID new_file_id_;
  Effect::FileID old_file_id_;
  char filename_[128];
#ifdef ENABLE_SD
  File sd_file_;
#endif
#ifdef ENABLE_SERIALFLASH
  SerialFlashFile sf_file_;
#endif
  int16_t* dest_ = nullptr;
  int to_read_ = 0;
  int tmp_;
  float start_ = 0.0;

  int rate_;
  uint8_t channels_;
  uint8_t bits_;

  bool wav_;

  size_t len_ = 0;
  volatile size_t sample_bytes_ = 0;
  unsigned char* ptr_;
  unsigned char* end_;
  unsigned char buffer[512 + 8]  __attribute__((aligned(4)));

  // Number of samples_ in samples that has been
  // sent out already.
  int written_ = 0;
  
  // Number of samples in samples_
  int num_samples_ = 0;
  int16_t samples_[32];
};

#endif
