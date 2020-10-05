#ifndef SOUND_PLAYWAV_H
#define SOUND_PLAYWAV_H

#include "../common/file_reader.h"
#include "../common/state_machine.h"
#include "audiostream.h"

// Simple upsampler code, doubles the number of samples with
// 2-lobe lanczos upsampling.
#define UPSCALE_C1 24757
#define UPSCALE_C2 -8191

#if 1
#define UPSAMPLE_FUNC(NAME, EMIT)                               \
  void NAME(int16_t sample) {                                   \
    upsample_buf_##NAME##_a_ = upsample_buf_##NAME##_b_;        \
    upsample_buf_##NAME##_b_ = upsample_buf_##NAME##_c_;        \
    upsample_buf_##NAME##_c_ = upsample_buf_##NAME##_d_;        \
    upsample_buf_##NAME##_d_ = sample;                          \
    EMIT(clamptoi16((upsample_buf_##NAME##_a_ * UPSCALE_C2 +            \
          upsample_buf_##NAME##_b_ * UPSCALE_C1 +                       \
          upsample_buf_##NAME##_c_ * UPSCALE_C1 +                       \
                   upsample_buf_##NAME##_d_ * UPSCALE_C2) >> 15));      \
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
    sample_bytes_ = 0;
    if (effect->Play(filename_)) {
      start_ = start;
      effect_ = nullptr;
      run_ = true;
    }
    PlayLoop(effect->GetFollowing());
  }
  void PlayLoop(Effect* effect) {
    effect_ = effect;
  }

  void Stop() override {
    noInterrupts();
    run_ = false;
    state_machine_.reset_state_machine();
    effect_ = nullptr;
    written_ = num_samples_ = 0;
    interrupts();
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
    if (bits == 8) return (*(ptr_++) << 8) - 32768;
    ptr_ += bits / 8 - 2;
    int16_t ret = *(int16_t*)ptr_;
    ptr_ += 2;
    return ret;
  }

  void AbortDecodeBytes(const char* why) {
    default_output->println(why);
    len_ = 0;
    to_read_ = 0;
    ptr_ = end_;
    run_ = false;
    effect_ = nullptr;
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
	AbortDecodeBytes("Unsupported rate.");
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
    else
      AbortDecodeBytes("Unsupported rate.");
  }

  template<int bits>
  void DecodeBytes2() {
    if (channels_ == 1) DecodeBytes3<bits, 1>();
    else if (channels_ == 2) DecodeBytes3<bits, 2>();
    else AbortDecodeBytes("unsupported number of channels");
  }

  void DecodeBytes() {
    if (bits_ == 8) DecodeBytes2<8>();
    else if (bits_ == 16) DecodeBytes2<16>();
//    else if (bits_ == 24) DecodeBytes2<24>();
//    else if (bits_ == 32) DecodeBytes2<32>();
    else AbortDecodeBytes("Unsupported sample size.");
  }

  int ReadFile(int n) {
    SCOPED_PROFILER();
    return file_.Read(buffer + 8, n);
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
	effect_ = effect_->GetFollowing();
      }
      if (new_file_id_ && new_file_id_ == old_file_id_) {
        // Minor optimization: If we're reading the same file
        // as before, then seek to 0 instead of open/close file.
        file_.Rewind();
      } else {
	if (!file_.OpenFast(filename_)) {
	  default_output->print("File ");
	  default_output->print(filename_);
	  default_output->println(" not found.");
	  goto fail;
	}
	YIELD();
        old_file_id_ = new_file_id_;
      }
      wav_ = endswith(".wav", filename_);
      if (wav_) {
        if (ReadFile(12) != 12) {
          default_output->println("Failed to read 12 bytes.");
          goto fail;
        }
        if (header(0) != 0x46464952 || header(2) != 0x45564157) {
          default_output->println("Not RIFF WAVE.");
          YIELD();
          goto fail;
        }

        // Look for FMT header.
        while (true) {
          if (ReadFile(8) != 8) {
            default_output->println("Failed to read 8 bytes.");
            goto fail;
          }

          len_ = header(1);
          if (header(0) != 0x20746D66) {  // 'fmt '
            file_.Skip(len_);
            continue;
          }
          if (len_ < 16) {
            default_output->println("FMT header is wrong size..");
            goto fail;
          }
          break;
        }
        
        if (16 != ReadFile(16)) {
          default_output->println("Read failed.");
          goto fail;
        }
        if (len_ > 16) file_.Skip(len_ - 16);
        if ((header(0) & 0xffff) != 1) {
          default_output->println("Wrong format.");
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
      default_output->print("channels: ");
      default_output->print(channels_);
      default_output->print(" rate: ");
      default_output->print(rate_);
      default_output->print(" bits: ");
      default_output->println(bits_);

      ptr_ = buffer + 8;
      end_ = buffer + 8;
      
      while (true) {
        if (wav_) {
          if (ReadFile(8) != 8) break;
          len_ = header(1);
          if (header(0) != 0x61746164) {
            file_.Skip(len_);
            continue;
          }
        } else {
          if (file_.Tell() >= file_.FileSize()) break;
          len_ = file_.FileSize() - file_.Tell();
        }
        sample_bytes_ = len_;

        if (start_ != 0.0) {
          int samples = Fmod(start_, length()) * rate_;
          int bytes_to_skip = samples * channels_ * bits_ / 8;
          file_.Skip(bytes_to_skip);
          len_ -= bytes_to_skip;
          start_ = 0.0;
        }

        while (len_) {
          {
            int bytes_read = ReadFile(file_.AlignRead(std::min<size_t>(len_, 512u)));
            if (bytes_read <= 0)
              break;
            len_ -= bytes_read;
            end_ = buffer + 8 + bytes_read;
          }
          while (ptr_ < end_ - channels_ * bits_ / 8) {
            DecodeBytes();

            while (written_ < num_samples_) {
              // Preload should go to here...
              while (to_read_ == 0) YIELD();

              int n = std::min<int>(num_samples_ - written_, to_read_);
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
    SCOPED_PROFILER();
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
    return (float)(sample_bytes_) * 8 / (bits_ * rate_ * channels_);
  }

  // Current position, seconds.
  float pos() const {
    if (!isPlaying()) return 0.0;
    return (float)(sample_bytes_ - len_ + end_ - ptr_) * 8 / (bits_ * rate_);
  }

  void Close() {
    file_.Close();
    old_file_id_ = new_file_id_ = Effect::FileID();
  }

  const char* filename() const {
    return filename_;
  }

private:
  volatile bool run_ = false;
  Effect* volatile effect_ = nullptr;
  Effect::FileID new_file_id_;
  Effect::FileID old_file_id_;
  char filename_[128];
  int16_t* dest_ = nullptr;
  int to_read_ = 0;
  int tmp_;
  float start_ = 0.0;

  int rate_;
  uint8_t channels_;
  uint8_t bits_;

  bool wav_;

  FileReader file_;

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
