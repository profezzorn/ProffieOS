#ifndef SOUND_AUDIO_STREAM_WORK_H
#define SOUND_AUDIO_STREAM_WORK_H


// AudioStreamWork is a linked list of classes that would like to
// do some work in a software-triggered interrupt. This is used to
// let audio processing preempt less important tasks.
#define IRQ_WAV 55

class AudioStreamWork;
AudioStreamWork* data_streams;

class AudioStreamWork {
public:
  AudioStreamWork() {
    next_ = data_streams;
    data_streams = this;
#ifdef TEENSYDUINO
    NVIC_SET_PRIORITY(IRQ_WAV, 240);
    _VectorsRam[IRQ_WAV + 16] = &ProcessAudioStreams;
    NVIC_ENABLE_IRQ(IRQ_WAV);
#endif    
  }
  ~AudioStreamWork() {
    for (AudioStreamWork** d = &data_streams; *d; d = &(*d)->next_) {
      if (*d == this) {
        *d = next_;
      }
    }
  }

  static void scheduleFillBuffer() {
#ifdef TEENSYDUINO
    if (!NVIC_IS_ACTIVE(IRQ_WAV))
      NVIC_TRIGGER_IRQ(IRQ_WAV);
#else
    armv7m_pendsv_enqueue((armv7m_pendsv_routine_t)ProcessAudioStreams, NULL, 0);
#endif    
  }


  static void LockSD(bool locked) {
//    scheduleFillBuffer();
    sd_locked = locked;
    if (locked) MountSDCard();
  }

  static bool sd_is_locked() { return sd_locked; }

protected:
  virtual bool FillBuffer() = 0;
  virtual size_t space_available() const = 0;

private:
  static void ProcessAudioStreams() {
    ScopedCycleCounter cc(wav_interrupt_cycles);
    if (sd_locked) return;
#if 1
    // Yes, it's a selection sort, luckily there's not a lot of
    // AudioStreamWork instances.
    for (int i = 0; i < 50; i++) {
      size_t max_space = 0;
      for (AudioStreamWork *d = data_streams; d; d=d->next_)
        max_space = max(max_space, d->space_available());
      if (max_space == 0) break;
      for (AudioStreamWork *d = data_streams; d; d=d->next_) {
        if (d->space_available() >= max_space)
          d->FillBuffer();
      }
    }
#else
    for (int i = 0; i < 10; i++) {
      for (AudioStreamWork *d = data_streams; d; d=d->next_) {
        d->FillBuffer();
      }
    }
#endif
  }


  static volatile bool sd_locked;
  AudioStreamWork* next_;
};

volatile bool AudioStreamWork::sd_locked = false;
#define LOCK_SD(X) AudioStreamWork::LockSD(X)

#endif
