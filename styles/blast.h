#ifndef STYLES_BLAST_H
#define STYLES_BLAST_H

// Usage: Blast<BASE, BLAST, FADEOUT_MS, WAVE_SIZE, WAVE_MS>
// Or: BlastL<BLAST, FADEOUT_MS, WAVE_SIZE, WAVE_MS>
// BASE, BLAST: COLOR
// FADEOUT_MS: a number (defaults to 150)
// WAVE_SIZE: a number (defaults to 100)
// WAVE_MS: a number (defaults to 400)
// return value: COLOR
// Normally shows BASE, but creates a blast effect using
// the color BLAST when a blast is requested. The effect
// is basically two humps moving out from the blast location.
// The size of the humps can be changed with WAVE_SIZE, note
// that smaller values makes the humps bigger. WAVE_MS determines
// how fast the waves travel. Smaller values makes the waves
// travel slower. Finally FADEOUT_MS determines how fast the
// humps fade back to the base color.

// Usage: BlastFadeout<BASE, BLAST, FADEOUT_MS>
// Or: BlastFadeoutL<BLAST, FADEOUT_MS>
// BASE, BLAST: COLOR
// FADEOUT_MS: a number (defaults to 250)
// return value: COLOR
// Normally shows BASE, but swiches to BLAST when a blast
// is requested and then fades back to BASE. FADEOUT_MS
// specifies out many milliseconds the fade takes.

// Usage: OriginalBlast<BASE, BLAST>
// Or: OriginalBlastL<BLAST>
// BASE, BLAST: COLOR
// return value: COLOR
// Normally shows BASE, but creates a blast effect using
// the color BLAST when a blast is requested.
// This was the original blast effect, but it is slow and not
// very configurable.

class BladeBase;

#include "alpha.h"
#include "layers.h"
#include "../functions/blast.h"

template<
  class BLAST,
  int FADEOUT_MS = 200,
  int WAVE_SIZE=100,
  int WAVE_MS=400,
  BladeEffectType EFFECT = EFFECT_BLAST>
  using BlastL = AlphaL<BLAST, BlastF<FADEOUT_MS, WAVE_SIZE, WAVE_MS, EFFECT>>;

template<class BASE,
  class BLAST,
  int FADEOUT_MS = 200,
  int WAVE_SIZE=100,
  int WAVE_MS=400,
  BladeEffectType EFFECT = EFFECT_BLAST>
  using Blast = Layers<BASE, BlastL<BLAST, FADEOUT_MS, WAVE_SIZE, WAVE_MS, EFFECT>>;


template<class BLAST, int FADEOUT_MS = 250, BladeEffectType EFFECT = EFFECT_BLAST>
  using BlastFadeoutL = AlphaL<BLAST, BlastFadeoutF<FADEOUT_MS, EFFECT>>;

template<class BASE, class BLAST, int FADEOUT_MS = 250, BladeEffectType EFFECT = EFFECT_BLAST>
  using BlastFadeout = Layers<BASE, BlastFadeoutL<BLAST, FADEOUT_MS, EFFECT>>;

template<class BLAST, BladeEffectType EFFECT=EFFECT_BLAST>
  using OriginalBlastL = AlphaL<BLAST, OriginalBlastF<EFFECT>>;

template<class BASE, class BLAST, BladeEffectType EFFECT=EFFECT_BLAST>
  using OriginalBlast = Layers<BASE, OriginalBlastL<BLAST, EFFECT>>;

#endif
