#ifndef STYLES_RESPONSIVE_STYLES_H
#define STYLES_RESPONSIVE_STYLES_H

// Responsive Effect/Style Aliases by Fett263,
// Responsive Effects use BladeAngle and TwistAngle to control effects.
// Defaults are set to simplify implementation.

// All of these styles are layers, use Layers<COLOR, ....> if you need on opaque color.

// ResponsiveLockupL<LOCKUP COLOR, TRANSITION1, TRANSITION2, TOP, BOTTOM, SIZE>
// Implements LocalizedLockup that will move based on the angle of the blade
// TRANSITION1 & TRANSITION2 = transition Begin & End
// TOP = uppermost lockup position limit, BOTTOM = lowermost lockup position limit, 32768 = tip, 0 = hilt
// SIZE controls LOCKUP area size 0 ~ 32768
template<class COLOR,
         class TR1 = TrInstant,
         class TR2 = TrInstant,
         class TOP = Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,
         class BOTTOM = Int<6000>,
         class SIZE = Scale<SwingSpeed<100>,Int<9000>,Int<14000>>>
using ResponsiveLockupL =
  LockupTrL<AlphaL<COLOR, Bump<Scale<BladeAngle<>,TOP,BOTTOM>,SIZE>>,
            TR1,
            TR2,
            SaberBase::LOCKUP_NORMAL>;

// ResponsiveDragL<DRAG COLOR, TRANSTION1, TRANSITION2, SIZE1, SIZE2>
// Implements Drag that will increase or decrease in size based on turning hilt
// TRANSITION1 & TRANSITION2 = transition Begin & End
// SIZE1 & SIZE2 control limits for DRAG size with TwistAngle
// LOCATION controls SmoothStep location
template<class COLOR,
         class TR1 = TrInstant,
         class TR2 = TrInstant,
         class SIZE1 = Int<2000>,
         class SIZE2 = Int<10000>,
         class LOCATION = Int<32000>>
using ResponsiveDragL =
  LockupTrL<AlphaL<COLOR, SmoothStep<LOCATION,Scale<TwistAngle<>,SIZE1,SIZE2>>>,
            TR1,
            TR2,
            SaberBase::LOCKUP_DRAG>;

// ResponsiveMeltL<MELT COLOR, TRANSITION1, TRANSITION2, SIZE1, SIZE2>
// Implements Melt effect for cutting through object, size will change to mimic metal
// heating and intensity will increase or decrease based on turning hilt
// TRANSITION1 & TRANSITION2 = transition Begin & End
// SIZE1 & SIZE2 control MELT area limits for TwistAngle
// LOCATION control SmoothStep location
template<class COLOR = Mix<TwistAngle<>,OrangeRed,Red>,
         class TR1 = TrWipeIn<600>,
         class TR2 = TrWipe<600>,
         class SIZE1 = Int<4000>,
         class SIZE2 = Int<10000>,
         class LOCATION = Int<30000>>
using ResponsiveMeltL =
  LockupTrL<AlphaL<COLOR, SmoothStep<LOCATION,Scale<TwistAngle<>,SIZE1,SIZE2>>>,
            TR1,
            TR2,
            SaberBase::LOCKUP_MELT>;

// ResponsiveLightningBlockL<LIGHTNING BLOCK COLOR, TRANSITION1, TRANSITION2>
// Implements hybrid Force Lightning Block with animation, intensity responds to turning the hilt and location/focus will respond to blade angle
// TRANSITION1 & TRANSITION2 = transition Begin & End
template<class COLOR, class TR1 = TrInstant, class TR2 = TrInstant>
using ResponsiveLightningBlockL =
  LockupTrL<
    AlphaL<COLOR,
      LayerFunctions<
         Bump<Scale<SlowNoise<Scale<BladeAngle<24000,32768>,Int<2100>,Int<1000>>>,Scale<BladeAngle<24000,32768>,Int<3000>,Int<10000>>,Int<16000>>,
                  Scale<BrownNoiseF<Int<10>>,Scale<TwistAngle<>,Int<4000>,Int<10000>>,Scale<TwistAngle<>,Int<9000>,Int<14000>>>>,
         Bump<Scale<SlowNoise<Int<2200>>,Scale<BladeAngle<24000,32768>,Int<26000>,Int<18000>>,Int<8000>>,
                  Scale<NoisySoundLevel,Scale<TwistAngle<>,Int<6000>,Int<10000>>,Scale<TwistAngle<>,Int<10000>,Int<14000>>>>,
         Bump<Scale<SlowNoise<Int<2300>>,Scale<BladeAngle<24000,32768>,Int<20000>,Int<16000>>,Scale<BladeAngle<24000,32768>,Int<30000>,Int<24000>>>,
                  Scale<IsLessThan<SlowNoise<Int<2000>>,Int<12000>>,Scale<NoisySoundLevel,Scale<TwistAngle<>,Int<9000>,Int<5000>>,Int<0>>,Int<0>>>>>,
    TR1,
    TR2,
    SaberBase::LOCKUP_LIGHTNING_BLOCK>;

// ResponsiveClashL<CLASH COLOR, TRANSITION1, TRANSITION2, TOP, BOTTOM, SIZE>
// Implements LocalizedClash effect that mimics ResponsiveLockup location and size
// TRANSITION1 & TRANSITION2 = transition Begin & End
// TOP = uppermost Clash position limit, BOTTOM = lowermost Clash position limit, 32768 = tip, 0 = hilt
// SIZE controls Clash area size 0 ~ 32768
template<class COLOR,
         class TR1 = TrInstant,
         class TR2 = TrFade<200>,
         class TOP = Scale<BladeAngle<0,16000>,Int<4000>,Int<26000>>,
         class BOTTOM = Int<6000>,
         class SIZE = Int<10000>>
using ResponsiveClashL =
  TransitionEffectL<TrConcat<TR1,
                             AlphaL<COLOR, Bump<Scale<BladeAngle<>,TOP,BOTTOM>,SIZE>>,
                             TR2>,
                    EFFECT_CLASH>;

// ResponsiveBlastL<BLAST COLOR, FADEOUT_MS, WAVE_SIZE, WAVE_SPEED, TOP, BOTTOM, EFFECT>
// Implements Blast effect that will move based on angle of the blade instead of random location Blast will impact and disperse along the blade from original position
// FADE = fade time ms
// WAVE_SIZE = size
// WAVE MS = speed ms
// TOP = uppermost Blast position limit, BOTTOM = lowermost Blast position limit, 32768 = tip, 0 = hilt
// EFFECT = effect type, defaults to EFFECT_BLAST

template<class COLOR,
         class FADE = Int<400>,
         class SIZE = Int<100>,
         class SPEED = Int<400>,
         class TOP = Int<28000>,
         class BOTTOM = Int<8000>,
         BladeEffectType EFFECT = EFFECT_BLAST>
using ResponsiveBlastL =
    AlphaL<
         MultiTransitionEffectL<
             TrWaveX<COLOR, FADE, SIZE, SPEED, Scale<BladeAngle<>, TOP, BOTTOM>>,
             EFFECT>,
         Bump<Scale<BladeAngle<>,TOP,BOTTOM>,Int<24000>>>;

// ResponsiveBlastWaveL<BLAST COLOR, FADEOUT_MS, WAVE_SIZE, WAVE_SPEED, TOP, BOTTOM, EFFECT>
// Implements Blast effect that will move based on angle of the blade instead of random location Blast will impact and split up and down the length of the blade from original position
// FADE = fade time ms
// WAVE_SIZE = size
// WAVE MS = speed ms
// TOP = uppermost Blast position limit, BOTTOM = lowermost Blast position limit, 32768 = tip, 0 = hilt
// EFFECT = effect type, defaults to EFFECT_BLAST

template<class COLOR,
         class FADE = Int<400>,
         class SIZE = Int<100>,
         class SPEED = Int<400>,
         class TOP = Int<28000>,
         class BOTTOM = Int<8000>,
         BladeEffectType EFFECT = EFFECT_BLAST>
using ResponsiveBlastWaveL =
    MultiTransitionEffectL<
         TrWaveX<COLOR, FADE, SIZE, SPEED,
              Scale<BladeAngle<>,
                    TOP,
                    BOTTOM>>,
         EFFECT>;

// ResponsiveBlastFadeL<BLAST COLOR, SIZE, FADE, TOP, BOTTOM, EFFECT>
// Implements Blast effect that will move based on angle of the blade instead of random location Blast will impact and Fade in position
// SIZE controls blast size bump 0 ~ 32768
// FADE = fade time ms
// TOP = uppermost Blast position limit, BOTTOM = lowermost Blast position limit, 32768 = tip, 0 = hilt
// EFFECT = effect type, defaults to EFFECT_BLAST

template<class COLOR,
         class SIZE = Int<8000>,
         class FADE = Int<400>,
         class TOP = Int<28000>,
         class BOTTOM = Int<8000>,
         BladeEffectType EFFECT = EFFECT_BLAST>
using ResponsiveBlastFadeL =
    MultiTransitionEffectL<
         TrConcat<TrInstant,
                  AlphaL<COLOR, Bump<Scale<BladeAngle<>, TOP, BOTTOM>, SIZE>>,
                  TrFadeX<FADE>>,
         EFFECT>;

// ResponsiveStabL<STAB COLOR, TRANSITION1, TRANSITION2, SIZE1, SIZE2>
// Stab effect
// Implements Stab effect that will change in size based on angle of the blade
// TRANSITION1 & TRANSITION2 = transition Begin & End
// SIZE1 & SIZE2 control Stab area limits for BladeAngle, 0 ~ 32768
// LOCATION control SmoothStep location
template<class COLOR,
         class TR1 = TrWipeIn<600>,
         class TR2 = TrWipe<600>,
         class SIZE1 = Int<14000>,
         class SIZE2 = Int<8000>,
         class LOCATION = Int<32000>>
using ResponsiveStabL =
  TransitionEffectL<TrConcat<TR1,
                             AlphaL<COLOR, SmoothStep<LOCATION,Scale<BladeAngle<>,SIZE1,SIZE2>>>,
                             TR2>,
                    EFFECT_STAB>;


#endif
