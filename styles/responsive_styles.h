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
         int TOP = 26000,
         int BOTTOM = 4000,
         int SIZE = 10000>
using ResponsiveLockupL =
  LockupTrL<AlphaL<COLOR, Bump<Scale<BladeAngle<>,Int<TOP>,Int<BOTTOM>>,Int<SIZE>>>,
            TR1,
            TR2,
            SaberBase::LOCKUP_NORMAL>; 

// ResponsiveDragL<DRAG COLOR, TRANSTION1, TRANSITION2, SIZE1, SIZE2>
// Implements Drag that will increase or decrease in size based on turning hilt
// TRANSITION1 & TRANSITION2 = transition Begin & End
// SIZE1 & SIZE2 control limits for DRAG size with TwistAngle
template<class COLOR, class TR1 = TrInstant, class TR2 = TrInstant, int SIZE1 = 2000, int SIZE2 = 10000>
using ResponsiveDragL =
  LockupTrL<AlphaL<COLOR, SmoothStep<Int<32000>,Scale<TwistAngle<>,Int<SIZE1>,Int<SIZE2>>>>,
            TR1,
            TR2,
            SaberBase::LOCKUP_DRAG>;

// ResponsiveMeltL<MELT COLOR, TRANSITION1, TRANSITION2, SIZE1, SIZE2>
// Implements Melt effect for cutting through object, size will change to mimic metal heating and intensity
// will increase or decrease based on turning hilt
// TRANSITION1 & TRANSITION2 = transition Begin & End
// SIZE1 & SIZE2 control MELT area limits for TwistAngle
template<class COLOR = Mix<TwistAngle<>,OrangeRed,Red>,
         class TR1 = TrWipeIn<600>,
         class TR2 = TrWipe<600>,
         int SIZE1 = 4000,
         int SIZE2 = 10000>
using ResponsiveMeltL =
  LockupTrL<AlphaL<COLOR, SmoothStep<Int<30000>,Scale<TwistAngle<>,Int<SIZE1>,Int<SIZE2>>>>,
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
         Bump<Sin<Int<24>,Scale<BladeAngle<>,Int<14000>,Int<6000>>,Int<4000>>,
              Sin<Int<17>,Scale<TwistAngle<>,Int<12000>,Int<9000>>,Int<8000>>>,
         Bump<Sin<Int<27>,Scale<BladeAngle<>,Int<24000>,Int<10000>>,Int<8000>>,
              Sin<Int<20>,Scale<TwistAngle<>,Int<10000>,Int<7000>>,Int<2000>>>,
         Bump<Sin<Int<25>,Int<18000>,Scale<BladeAngle<24000,32768>,Int<22000>,Int<15000>>>,
              Sin<Int<10>,Scale<TwistAngle<>,Int<6000>,Int<1000>>,Int<0>>>,
         Bump<Sin<Int<31>,Scale<BladeAngle<22000,32768>,Int<30000>,Scale<BladeAngle<24000,32768>,Int<16000>,Int<10000>>>,Scale<BladeAngle<24000,32768>,Int<16000>,Int<8000>>>,
              Sin<Int<18>,Scale<TwistAngle<>,Int<8000>,Int<2000>>,Int<0>>>>>, 
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
         int TOP = 22000,
         int BOTTOM = 8000,
         int SIZE = 10000>
using ResponsiveClashL =
  TransitionEffectL<AlphaL<COLOR, Bump<Scale<BladeAngle<>,Int<TOP>,Int<BOTTOM>>,Int<SIZE>>>,
                    TR1,
                    TR2,
                    EFFECT_CLASH>;

// ResponsiveBlastL<BLAST COLOR, TRANSITION1, TRANSITION2, TOP, BOTTOM, SIZE>
// Implements Blast effect that will move based on angle of the blade instead of random location
// TRANSITION1 & TRANSITION2 = transition Begin & End
// TOP = uppermost Blast position limit, BOTTOM = lowermost Blast position limit, 32768 = tip, 0 = hilt
// SIZE controls blast size 0 ~ 32768
template<class COLOR,
         class TR1 = TrInstant,
         class TR2 = TrFade<200>,
         int TOP = 28000,
         int BOTTOM = 8000,
         int SIZE = 8000>
using ResponsiveBlastL =
  MultiTransitionEffectL<AlphaL<COLOR, Bump<Scale<BladeAngle<>,Int<TOP>,Int<BOTTOM>>,Int<SIZE>>>,
                         TR1,
                         TR2,
                         EFFECT_BLAST>;

// ResponsiveStabL<STAB COLOR, TRANSITION1, TRANSITION2, SIZE1, SIZE2>
// Stab effect
// Implements Stab effect that will change in size based on angle of the blade
// TRANSITION1 & TRANSITION2 = transition Begin & End
// SIZE1 & SIZE2 control Stab area limits for BladeAngle, 0 ~ 32768
template<class COLOR,
         class TR1 = TrWipeIn<600>,
         class TR2 = TrWipe<600>,
         int SIZE1 = 14000,
         int SIZE2 = 8000>
using ResponsiveStabL =
  TransitionEffectL<AlphaL<COLOR, SmoothStep<Int<32000>,Scale<BladeAngle<>,Int<SIZE1>,Int<SIZE2>>>>,
                    TR1,
                    TR2,
                    EFFECT_STAB>;


#endif
