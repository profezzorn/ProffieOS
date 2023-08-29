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

RefPtr<BufferedWavPlayer> GetWavPlayerPlaying(const Effect* effect) {
  for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
    if (wav_players[unit].isPlaying() &&
	wav_players[unit].current_file_id().GetEffect() == effect) {
      return RefPtr<BufferedWavPlayer>(wav_players + unit);
    }
  }
  return RefPtr<BufferedWavPlayer>();
}

#ifdef KILL_OLD_PLAYERS
RefPtr<BufferedWavPlayer> GetOrFreeWavPlayer(Effect* e)  {
  if (!e->GetFollowing() && !e->GetKillable() && GetWavPlayerPlaying(e)) {
    STDERR << "MAKING " << e->GetName() << " killable.\n";
    e->SetKillable(true);
  }
  RefPtr<BufferedWavPlayer> ret = GetFreeWavPlayer();
  if (ret) return ret;

  BufferedWavPlayer* p = nullptr;
  float best_remaining = 1000.0;
  for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
    if (wav_players[unit].isPlaying() &&
	wav_players[unit].refs() == 0 &&
	wav_players[unit].current_file_id().GetEffect() &&
	wav_players[unit].current_file_id().GetEffect()->GetKillable()) {
      float remaining = wav_players[unit].length() - wav_players[unit].pos();
      if (remaining < best_remaining) {
	best_remaining = remaining;
	p = wav_players + unit;
      }
    }
  }
  if (p) {
    STDERR << "KILLING PLAYER " << WhatUnit(p) << "\n";
    p->set_fade_time(0.001);
    p->FadeAndStop();
    while (p->isPlaying()) {
#if VERSION_MAJOR >= 4
      armv7m_core_yield();
#endif
    }
    return RefPtr<BufferedWavPlayer>(p);
  }
  return RefPtr<BufferedWavPlayer>();
}
#else
RefPtr<BufferedWavPlayer> GetOrFreeWavPlayer(Effect* e)  {
  return GetFreeWavPlayer();
}
#endif

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
#ifndef DISABLE_TALKIE  
  dynamic_mixer.streams_[NELEM(wav_players)+1] = &talkie;
#endif
}

void SetupStandardAudio() {
  dac.SetStream(NULL);
  SetupStandardAudioLow();
  dac.SetStream(&dynamic_mixer);
}

void SaySDInitError(int error) {
#ifndef DISABLE_TALKIE
  talkie.Say(spS);
  talkie.Say(spD);
  talkie.Say(spSTART);
  talkie.SayNumber(error);
#endif  
}

void SaySDCheckError(int error) {
#ifndef DISABLE_TALKIE
  talkie.Say(spS);
  talkie.Say(spD);
  talkie.Say(spCHECK);
  talkie.SayNumber(error);
#endif
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
#include "effect.h"

#endif  // ENABLE_AUDIO
