#ifndef COMMON_SABER_BASE_H
#define COMMON_SABER_BASE_H

#include "linked_list.h"
#include "vec3.h"

// SaberBase is our main class for distributing saber-related events, such
// as on/off/clash/etc. to where they need to go. Each SABERFUN below
// has a corresponding SaberBase::Do* function which invokes that function
// on all active SaberBases.
class SaberBase;
extern SaberBase* saberbases;


// An effect is a one-shot trigger for something
// Effects may have a location, but do not generally have a "duration".
// However, durations may be derived from blade styles or sound effect durations
// associated with the effect.
#define DEFINE_ALL_EFFECTS()			\
    DEFINE_EFFECT(NONE)				\
    DEFINE_EFFECT(CLASH)			\
    DEFINE_EFFECT(CLASH_UPDATE)			\
    DEFINE_EFFECT(BLAST)			\
    DEFINE_EFFECT(FORCE)			\
    DEFINE_EFFECT(STAB)				\
    DEFINE_EFFECT(BOOT)				\
    DEFINE_EFFECT(LOCKUP_BEGIN)			\
    DEFINE_EFFECT(LOCKUP_END)			\
    DEFINE_EFFECT(DRAG_BEGIN)			\
    DEFINE_EFFECT(DRAG_END)			\
    DEFINE_EFFECT(PREON)			\
    DEFINE_EFFECT(POSTOFF)			\
    DEFINE_EFFECT(IGNITION)			\
    DEFINE_EFFECT(RETRACTION)			\
    DEFINE_EFFECT(CHANGE)			\
    DEFINE_EFFECT(NEWFONT)			\
    DEFINE_EFFECT(LOW_BATTERY)			\
    DEFINE_EFFECT(POWERSAVE)                    \
    DEFINE_EFFECT(BATTERY_LEVEL)                \
    DEFINE_EFFECT(VOLUME_LEVEL)                 \
    /* Allows style to turn blade ON for interactive effects if prop/style support, FAST_ON skips PREON. */          \
    DEFINE_EFFECT(ON)                           \
    DEFINE_EFFECT(FAST_ON)                      \
    DEFINE_EFFECT(QUOTE)			\
    DEFINE_EFFECT(SECONDARY_IGNITION)		\
    DEFINE_EFFECT(SECONDARY_RETRACTION)		\
    /* Allows style to turn blade OFF for interactive effects if prop/style support, FAST_OFF skips POSTOFF. */          \
    DEFINE_EFFECT(OFF)                          \
    DEFINE_EFFECT(FAST_OFF)                     \
    DEFINE_EFFECT(OFF_CLASH)                    \
    DEFINE_EFFECT(NEXT_QUOTE)                   \
    DEFINE_EFFECT(INTERACTIVE_PREON)            \
    /* Triggers a Blaster sound to interact with and creates an EFFECT_BLAST if prop/style support. */          \
    DEFINE_EFFECT(INTERACTIVE_BLAST)            \
    DEFINE_EFFECT(TRACK)			\
    DEFINE_EFFECT(BEGIN_BATTLE_MODE)            \
    DEFINE_EFFECT(END_BATTLE_MODE)              \
    DEFINE_EFFECT(BEGIN_AUTO_BLAST)             \
    DEFINE_EFFECT(END_AUTO_BLAST)               \
    /* Triggers the change for sets of sounds within the font from one alternative to another. */                \
    DEFINE_EFFECT(ALT_SOUND)			\
    /* Triggers an optional sound effect during transitions from within a style via TrDoEffect. */         \
    DEFINE_EFFECT(TRANSITION_SOUND)		\
    /* Toggles an optonal sound effect loop ON/OFF from within a style via TrDoEffect. */          \
    DEFINE_EFFECT(SOUND_LOOP)                   \
    /* Blaster effects */                       \
    DEFINE_EFFECT(STUN)				\
    DEFINE_EFFECT(FIRE)				\
    DEFINE_EFFECT(CLIP_IN)			\
    DEFINE_EFFECT(CLIP_OUT)			\
    DEFINE_EFFECT(RELOAD)			\
    DEFINE_EFFECT(MODE)				\
    DEFINE_EFFECT(RANGE)			\
    DEFINE_EFFECT(EMPTY)			\
    DEFINE_EFFECT(FULL)				\
    DEFINE_EFFECT(JAM)				\
    DEFINE_EFFECT(UNJAM)			\
    DEFINE_EFFECT(PLI_ON)			\
    DEFINE_EFFECT(PLI_OFF)                      \
    /* Mini game effects */                     \
    DEFINE_EFFECT(GAME_START)                   \
    DEFINE_EFFECT(GAME_ACTION1)                 \
    DEFINE_EFFECT(GAME_ACTION2)                 \
    DEFINE_EFFECT(GAME_CHOICE)                  \
    DEFINE_EFFECT(GAME_RESPONSE1)               \
    DEFINE_EFFECT(GAME_RESPONSE2)               \
    DEFINE_EFFECT(GAME_RESULT1)                 \
    DEFINE_EFFECT(GAME_RESULT2)                 \
    DEFINE_EFFECT(GAME_WIN)                     \
    DEFINE_EFFECT(GAME_LOSE)                    \
    /* user-definable effects */                \
    DEFINE_EFFECT(USER1)			\
    DEFINE_EFFECT(USER2)			\
    DEFINE_EFFECT(USER3)			\
    DEFINE_EFFECT(USER4)			\
    DEFINE_EFFECT(USER5)			\
    DEFINE_EFFECT(USER6)			\
    DEFINE_EFFECT(USER7)			\
    DEFINE_EFFECT(USER8)                        \
    /* ERRORS */                                \
    DEFINE_EFFECT(SD_CARD_NOT_FOUND)            \
    DEFINE_EFFECT(ERROR_IN_FONT_DIRECTORY)      \
    DEFINE_EFFECT(ERROR_IN_BLADE_ARRAY)         \
    DEFINE_EFFECT(FONT_DIRECTORY_NOT_FOUND)     \


#define DEFINE_EFFECT(X) EFFECT_##X,
enum class EffectType {
  DEFINE_ALL_EFFECTS()
};
enum class EffectTypeHelper {
  DEFINE_ALL_EFFECTS()
  NUMBER_OF_EFFECTS
};
#undef DEFINE_EFFECT

#define DEFINE_EFFECT(X) constexpr EffectType EFFECT_##X=EffectType::EFFECT_##X;
DEFINE_ALL_EFFECTS();

// This can be used if we need to allocate an array with one entry per effect
constexpr size_t NUMBER_OF_EFFECTS = (size_t)EffectTypeHelper::NUMBER_OF_EFFECTS;
#undef DEFINE_EFFECT

class SaberBase {
protected:
  void Link(SaberBase* x) {
    CHECK_LL(SaberBase, saberbases, next_saber_);
    noInterrupts();
    x->next_saber_ = saberbases;
    saberbases = x;
    interrupts();
    CHECK_LL(SaberBase, saberbases, next_saber_);
  }
  void Unlink(const SaberBase* x) {
    CHECK_LL(SaberBase, saberbases, next_saber_);
    for (SaberBase** i = &saberbases; *i; i = &(*i)->next_saber_) {
      if (*i == x) {
        *i = x->next_saber_;
        CHECK_LL(SaberBase, saberbases, next_saber_);
        return;
      }
    }
    CHECK_LL(SaberBase, saberbases, next_saber_);
  }

  SaberBase() { Link(this); }
  explicit SaberBase(NoLink _) {}
  ~SaberBase() { Unlink(this); }

public:
  enum OffType {
    OFF_NORMAL,
    OFF_FAST,
    OFF_BLAST,
    OFF_IDLE,
    OFF_CANCEL_PREON,
  };

  static bool IsOn() { return on_; }
  static void TurnOn() {
    on_ = true;
    SaberBase::DoOn();
  }
  static void TurnOff(OffType off_type) {
    on_ = false;
    last_motion_request_ = millis();
    SaberBase::DoOff(off_type);
  }

  static bool MotionRequested() {
#if NUM_BUTTONS == 0
    return true;
#else
    return IsOn() || (millis() - last_motion_request_) < 20000;
#endif
  }
  static void RequestMotion() {
    last_motion_request_ = millis();
  }

  static void DumpMotionRequest() {
    STDOUT << "Motion requested: " << MotionRequested()
	   << " (millis() - last_motion_request=" << (millis() - last_motion_request_)
	   << ")\n";
  }

  enum LockupType {
    LOCKUP_NONE,
    LOCKUP_NORMAL,
    LOCKUP_DRAG,
    LOCKUP_ARMED,   // For detonators and such
    LOCKUP_AUTOFIRE, // For blasters and phasers
    LOCKUP_MELT,     // For cutting through doors...
    LOCKUP_LIGHTNING_BLOCK,  // Lightning block lockup
  };
  static LockupType Lockup() { return lockup_; }
  static void SetLockup(LockupType lockup) { lockup_ = lockup; }

  enum ChangeType {
    ENTER_COLOR_CHANGE,
    EXIT_COLOR_CHANGE,
    CHANGE_COLOR,
  };

  // 1.0 = kDefaultVolume
  // This is really just for sound fonts.
  virtual void SetHumVolume(float volume) {}
  virtual void StartSwing(const Vec3& gyro, float swingThreshold, float slashThreshold) {}
  virtual float SetSwingVolume(float swing_strength, float mixhum) {
    return mixhum;
  }

  static float sound_length;
  static int sound_number;

  static void ClearSoundInfo() {
    sound_length = 0.0;
    sound_number = -1;
  }

#define SABERFUN(NAME, EFFECT, TYPED_ARGS, ARGS)		\
public:                                                         \
  static void Do##NAME TYPED_ARGS {                             \
    ClearSoundInfo();				                \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {    \
      p->SB_##NAME ARGS;                                        \
    }                                                           \
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {    \
      p->SB_##NAME##2 ARGS;                                     \
    }                                                           \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
  }                                                             \
                                                                \
  virtual void SB_##NAME TYPED_ARGS {}                          \
  virtual void SB_##NAME##2 TYPED_ARGS {}

#define SABERBASEFUNCTIONS()						\
  SABERFUN(Effect, effect, (EffectType effect, float location), (effect, location)); \
  SABERFUN(On, EFFECT_IGNITION, (), ());				\
  SABERFUN(Off, EFFECT_RETRACTION, (OffType off_type), (off_type));	\
  SABERFUN(BladeDetect, EFFECT_NONE, (bool detected), (detected));	\
  SABERFUN(Change, EFFECT_CHANGE, (ChangeType change_type), (change_type)); \
									\
  SABERFUN(Top, EFFECT_NONE, (uint64_t total_cycles), (total_cycles));	\
  SABERFUN(IsOn, EFFECT_NONE, (bool* on), (on));			\
  
  SABERBASEFUNCTIONS();

  static void DoEffect(EffectType e, float location, int N) {
    sound_length = 0.0;
    sound_number = N;
    CHECK_LL(SaberBase, saberbases, next_saber_);
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {
      p->SB_Effect(e, location);
    }
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {
      p->SB_Effect2(e, location);
    }
    CHECK_LL(SaberBase, saberbases, next_saber_);
  }
  static void DoEffectR(EffectType e) { DoEffect(e, (200 + random(700))/1000.0f); }
  static void DoBlast() { DoEffectR(EFFECT_BLAST); }
  static void DoForce() { DoEffectR(EFFECT_FORCE); }
  static void DoBoot() { DoEffect(EFFECT_BOOT, 0); }
  static void DoPreOn() { DoEffect(EFFECT_PREON, 0); }
  static void DoBeginLockup() { DoEffectR(EFFECT_LOCKUP_BEGIN); }
  static void DoEndLockup() { DoEffect(EFFECT_LOCKUP_END, 0); }
  static void DoChange() { DoEffect(EFFECT_CHANGE, 0); }
  static void DoNewFont() { DoEffect(EFFECT_NEWFONT, 0); }
  static void DoLowBatt() { DoEffect(EFFECT_LOW_BATTERY, 0); }

  static float clash_strength_;

  // Note, the full clash strength might not be known
  // at the time that the EFFECT_CLASH event is emitted.
  // In general, the full strength will be known a few
  // milliseconds later. Every time the clash strength
  // increases we also emit an EFFECT_CLASH_UPDATE event.
  static float GetClashStrength() {
    return clash_strength_;
  }

  // Strength is for all types of clashes.
  void SetClashStrength(float strength) {
    clash_strength_ = strength;
  }
  static void DoClash() {
    DoEffectR(EFFECT_CLASH);
  }
  static void DoStab() {
    DoEffect(EFFECT_STAB, 1.0f);
  }
  static void UpdateClashStrength(float strength) {
    if (strength > clash_strength_) {
      clash_strength_ = strength;
      DoEffect(EFFECT_CLASH_UPDATE, strength);
    }
  }
  
#undef SABERFUN

  /* Swing rotation speed degrees per second */
  static void DoMotion(Vec3 gyro, bool clear) {
#ifdef INVERT_ORIENTATION
    gyro.x = -gyro.x;
#endif
    CHECK_LL(SaberBase, saberbases, next_saber_);
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {
      p->SB_Motion(gyro, clear);
    }
    CHECK_LL(SaberBase, saberbases, next_saber_);
  }
  virtual void SB_Motion(const Vec3& gyro, bool clear) {}

  /* Acceleration in g */
  static void DoAccel(Vec3 gyro, bool clear) {
#ifdef INVERT_ORIENTATION
    gyro.x = -gyro.x;
#endif
    CHECK_LL(SaberBase, saberbases, next_saber_);
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {
      p->SB_Accel(gyro, clear);
    }
    CHECK_LL(SaberBase, saberbases, next_saber_);
  }
  virtual void SB_Accel(const Vec3& gyro, bool clear) {}

  static uint32_t GetCurrentVariation() {
    return current_variation_;
  }

  // For step-wise updates
  static void UpdateVariation(int delta) {
    current_variation_ += delta;
    DoChange(CHANGE_COLOR);
  }
  // For smooth updates or restore.
  static void SetVariation(uint32_t v) {
    current_variation_ = v;
  }

#ifdef DYNAMIC_BLADE_DIMMING
  static int dimming_;
  static int GetCurrentDimming() { return dimming_; }
  static void SetDimming(int dimming) { dimming_ = dimming; }
#endif  

  enum ColorChangeMode {
    COLOR_CHANGE_MODE_NONE,
    COLOR_CHANGE_MODE_STEPPED,
    COLOR_CHANGE_MODE_SMOOTH,
    COLOR_CHANGE_MODE_ZOOMED,
  };

  static ColorChangeMode GetColorChangeMode() { return color_change_mode_; }
  static void SetColorChangeMode(ColorChangeMode  mode) {
    ColorChangeMode prev_mode = color_change_mode_;
    color_change_mode_ = mode;
    if (mode == COLOR_CHANGE_MODE_NONE) {
      DoChange(EXIT_COLOR_CHANGE);
    } else if (prev_mode == COLOR_CHANGE_MODE_NONE) {
      DoChange(ENTER_COLOR_CHANGE);
    }
  }

  // Not private for debugging purposes only.
  static uint32_t last_motion_request_;
private:
  static bool on_;
  static LockupType lockup_;
  static uint32_t current_variation_;
  static ColorChangeMode color_change_mode_;
  SaberBase* next_saber_;
};

#endif
