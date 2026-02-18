#ifndef SOUND_AUDIO_STREAM_WORK_H
#define SOUND_AUDIO_STREAM_WORK_H

#include "../common/atomic.h"
#include "../common/circular_buffer.h"

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

#ifdef ESP32

  static void FillBufferTask(void* unused) {
    while (true) {
      ProcessAudioStreams();
    }
  }

  static POAtomic<bool> task_created_;
  static void scheduleFillBuffer() {
    if (!task_created_.exchange(true)) {
      TaskHandle_t xHandle = NULL;
      xTaskCreatePinnedToCore(FillBufferTask, "SDReadTask", 20000, nullptr, tskIDLE_PRIORITY, &xHandle, 0);
    }
    fill_buffers_pending_.set(true);
  }

#else
  static void scheduleFillBuffer() {
    bool enqueue = false;
    noInterrupts();
    if (!fill_buffers_pending_.get()) {
      fill_buffers_pending_.set(true);
      enqueue = true;
    }
    interrupts();
    if (enqueue) {
#ifdef TEENSYDUINO
      NVIC_TRIGGER_IRQ(IRQ_WAV);
#elif defined(ARDUINO_ARCH_STM32L4)
      armv7m_pendsv_enqueue((armv7m_pendsv_routine_t)ProcessAudioStreams, NULL, 0);
#else
      // TODO
#endif    
    }
  }
#endif  // ESP32

  static void LockSD(bool locked) {
//    scheduleFillBuffer();
    sd_locked += locked ? 1 : -1;
    if (locked) MountSDCard();
  }

  static void LockSD_nomount(bool locked) {
    sd_locked += locked ? 1 : -1;
  }
  
  static bool sd_is_locked() { return sd_locked.get() > 0; }

  static void CloseAllOpenFiles() {
    for (AudioStreamWork *d = data_streams; d; d=d->next_)
      d->CloseFiles();
  }
  static bool SDActive() {
    for (AudioStreamWork *d = data_streams; d; d=d->next_)
      if (d->IsActive())
	return true;
    return false;
  }
protected:
  virtual bool FillBuffer() = 0;
  virtual bool IsActive() { return false; }
  virtual void CloseFiles() = 0;
  virtual size_t space_available() = 0;

private:
  static void ProcessAudioStreams() {
    ScopedCycleCounter cc(wav_interrupt_cycles);
    if (sd_locked.get()) {
      fill_buffers_pending_.set(false);
      return;
    }
    Looper::CheckFrozen();
#if 1
    // Yes, it's a selection sort, luckily there's not a lot of
    // AudioStreamWork instances.
    int i;
    for (i = 0; i < 50; i++) {
      size_t max_space = 0;
      for (AudioStreamWork *d = data_streams; d; d=d->next_) {
	size_t space = d->space_available();
	if (i == 0) TRACE2(SWIRQ, "space=", space);
        max_space = std::max(max_space, space);
      }
      TRACE2(SWIRQ, "max_space=", max_space);
      if (max_space == 0) break;
      for (AudioStreamWork *d = data_streams; d; d=d->next_) {
        if (d->space_available() >= max_space) {
          d->FillBuffer();
	  TRACE2(SWIRQ, "after=", d->space_available());
	}
      }
    }
    TRACE2(SWIRQ, "i=", i);
#else
    for (int i = 0; i < 10; i++) {
      for (AudioStreamWork *d = data_streams; d; d=d->next_) {
        d->FillBuffer();
      }
    }
#endif
    fill_buffers_pending_.set(false);
  }

  static POAtomic<uint32_t> sd_locked;
  static POAtomic<bool> fill_buffers_pending_;
  AudioStreamWork* next_;
};

POAtomic<uint32_t> AudioStreamWork::sd_locked (false);
POAtomic<bool> AudioStreamWork::fill_buffers_pending_(false);
#define LOCK_SD(X) AudioStreamWork::LockSD(X)

#ifdef ESP32
POAtomic<bool> AudioStreamWork::task_created_ (false);
#endif

#endif
