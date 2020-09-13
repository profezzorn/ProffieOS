#ifdef ENABLE_AUDIO

// DMA-driven audio output.
#define AUDIO_BUFFER_SIZE 44
#define AUDIO_RATE 44100
#define NUM_WAV_PLAYERS 7

#include "click_avoider_lin.h"
#include "waveform_sampler.h"
#include "audiostream.h"
#include "dynamic_mixer.h"
#include "dac.h"
#include "beeper.h"
#include "talkie.h"
#include "lightsaber_synth.h"

Beeper beeper;
Talkie talkie;

// LightSaberSynth saber_synth;
#include "buffered_audio_stream.h"

size_t WhatUnit(class BufferedWavPlayer* player);

#include "effect.h"
#include "buffered_wav_player.h"

BufferedWavPlayer wav_players[NUM_WAV_PLAYERS];
RefPtr<BufferedWavPlayer> track_player_;

RefPtr<BufferedWavPlayer> GetFreeWavPlayer()  {
  // Find a free wave playback unit.
  for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
    if (wav_players[unit].Available()) {
      wav_players[unit].reset_volume();
      return RefPtr<BufferedWavPlayer>(wav_players + unit);
    }
  }
  return RefPtr<BufferedWavPlayer>();
}

RefPtr<BufferedWavPlayer> RequireFreeWavPlayer()  {
  while (true) {
    RefPtr<BufferedWavPlayer> ret = GetFreeWavPlayer();
    if (ret) return ret;
    STDOUT.println("Failed to get hum player, trying again!");
    delay(100);
  }
}

size_t WhatUnit(class BufferedWavPlayer* player) {
  if (!player) return -1;
  return player - wav_players;
}

void SetupStandardAudioLow() {
//  audio_splicer.Deactivate();
  for (size_t i = 0; i < NELEM(wav_players); i++) {
    if (wav_players[i].refs() != 0) {
      STDOUT.println("WARNING, wav player still referenced!");
    }
    dynamic_mixer.streams_[i] = wav_players + i;
    wav_players[i].reset_volume();
  }
  dynamic_mixer.streams_[NELEM(wav_players)] = &beeper;
  dynamic_mixer.streams_[NELEM(wav_players)+1] = &talkie;
}

void SetupStandardAudio() {
  dac.SetStream(NULL);
  SetupStandardAudioLow();
  dac.SetStream(&dynamic_mixer);
}


#include "../common/config_file.h"
#include "hybrid_font.h"

HybridFont hybrid_font;

#include "smooth_swing_config.h"
#include "smooth_swing_cfx_config.h"
#include "looped_swing_wrapper.h"
#include "smooth_swing_v2.h"

LoopedSwingWrapper looped_swing_wrapper;
SmoothSwingV2 smooth_swing_v2;

#else  // ENABLE_AUDIO

#define LOCK_SD(X) do { } while(0)
#include "../common/sd_card.h"

#endif  // ENABLE_AUDIO
