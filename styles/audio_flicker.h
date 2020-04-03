#ifndef STYLES_AUDIO_FLICKER_H
#define STYLES_AUDIO_FLICKER_H

// Usage: AudioFlicker<A, B>
// Or: AudioFlickerL<B>
// A, B: COLOR
// return value: COLOR
// Mixes between A and B based on audio. Quiet audio
// means more A, loud audio means more B.
// Based on a single sample instead of an average to make it flicker.

class BladeBase;

#include "alpha.h"
#include "../functions/sound_level.h"
#include "layers.h"

template<class B>
using AudioFlickerL = AlphaL<B, NoisySoundLevelCompat>;

template<class A, class B>
using AudioFlicker = Layers<A, AudioFlickerL<B>>;

#endif
