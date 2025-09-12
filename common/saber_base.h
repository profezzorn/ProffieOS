#ifndef COMMON_SABER_BASE_H
#define COMMON_SABER_BASE_H

#include "linked_list.h"
#include "vec3.h"
#include "onceperblade.h"

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
#define DEFINE_ALL_EFFECTS()                    \
    DEFINE_EFFECT(NONE)                         \
    DEFINE_EFFECT(CLASH)                        \
    DEFINE_EFFECT(CLASH_UPDATE)                 \
    DEFINE_EFFECT(BLAST)                        \
    DEFINE_EFFECT(FORCE)                        \
    DEFINE_EFFECT(STAB)                         \
    DEFINE_EFFECT(BOOT)                         \
    DEFINE_EFFECT(LOCKUP_BEGIN)                 \
    DEFINE_EFFECT(LOCKUP_END)                   \
    DEFINE_EFFECT(DRAG_BEGIN)                   \
    DEFINE_EFFECT(DRAG_END)                     \
    DEFINE_EFFECT(PREON)                        \
    DEFINE_EFFECT(POSTOFF)                      \
    DEFINE_EFFECT(IGNITION)                     \
    DEFINE_EFFECT(RETRACTION)                   \
    DEFINE_EFFECT(CHANGE)                       \
    /* just for resetting things, not meant for triggering effects, use NEWFONT/BLADEIN/BLADEOUT instead */ \
    DEFINE_EFFECT(CHDIR)                        \
    DEFINE_EFFECT(NEWFONT)                      \
    DEFINE_EFFECT(BLADEIN)                      \
    DEFINE_EFFECT(BLADEOUT)                     \
    DEFINE_EFFECT(LOW_BATTERY)                  \
    DEFINE_EFFECT(POWERSAVE)                    \
    DEFINE_EFFECT(BATTERY_LEVEL)                \
    DEFINE_EFFECT(VOLUME_LEVEL)                 \
    DEFINE_EFFECT(ACCENT_SWING)                 \
    DEFINE_EFFECT(ACCENT_SLASH)                 \
    DEFINE_EFFECT(SPIN)                         \
    /* Allows style to turn blade ON for interactive effects if prop/style support, FAST_ON skips PREON. */         \
    DEFINE_EFFECT(ON)                           \
    DEFINE_EFFECT(FAST_ON)                      \
    DEFINE_EFFECT(QUOTE)                        \
    DEFINE_EFFECT(SECONDARY_IGNITION)           \
    DEFINE_EFFECT(SECONDARY_RETRACTION)         \
    /* Allows style to turn blade OFF for interactive effects if prop/style support, FAST_OFF skips POSTOFF. */     \
    DEFINE_EFFECT(OFF)                          \
    DEFINE_EFFECT(FAST_OFF)                     \
    DEFINE_EFFECT(OFF_CLASH)                    \
    DEFINE_EFFECT(NEXT_QUOTE)                   \
    DEFINE_EFFECT(INTERACTIVE_PREON)            \
    /* Triggers a Blaster sound to interact with and creates an EFFECT_BLAST if prop/style support. */             \
    DEFINE_EFFECT(INTERACTIVE_BLAST)            \
    DEFINE_EFFECT(TRACK)                        \
    DEFINE_EFFECT(BEGIN_BATTLE_MODE)            \
    DEFINE_EFFECT(END_BATTLE_MODE)              \
    DEFINE_EFFECT(BEGIN_AUTO_BLAST)             \
    DEFINE_EFFECT(END_AUTO_BLAST)               \
    /* Triggers the change for sets of sounds within the font from one alternative to another. */                  \
    DEFINE_EFFECT(ALT_SOUND)                    \
    /* Triggers an optional sound effect during transitions from within a style via TrDoEffect. */                 \
    DEFINE_EFFECT(TRANSITION_SOUND)             \
    /* Toggles an optonal sound effect loop ON/OFF from within a style via TrDoEffect. */                          \
    DEFINE_EFFECT(SOUND_LOOP)                   \
    /* Blaster effects */                       \
    DEFINE_EFFECT(STUN)                         \
    DEFINE_EFFECT(FIRE)                         \
    DEFINE_EFFECT(CLIP_IN)                      \
    DEFINE_EFFECT(CLIP_OUT)                     \
    DEFINE_EFFECT(RELOAD)                       \
    DEFINE_EFFECT(MODE)                         \
    DEFINE_EFFECT(RANGE)                        \
    DEFINE_EFFECT(EMPTY)                        \
    DEFINE_EFFECT(FULL)                         \
    DEFINE_EFFECT(JAM)                          \
    DEFINE_EFFECT(UNJAM)                        \
    DEFINE_EFFECT(PLI_ON)                       \
    DEFINE_EFFECT(PLI_OFF)                      \
    DEFINE_EFFECT(DESTRUCT)                     \
    DEFINE_EFFECT(BOOM)                         \
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
    DEFINE_EFFECT(USER1)                        \
    DEFINE_EFFECT(USER2)                        \
    DEFINE_EFFECT(USER3)                        \
    DEFINE_EFFECT(USER4)                        \
    DEFINE_EFFECT(USER5)                        \
    DEFINE_EFFECT(USER6)                        \
    DEFINE_EFFECT(USER7)                        \
    DEFINE_EFFECT(USER8)                        \
    /* ERRORS */                                \
    DEFINE_EFFECT(SD_CARD_NOT_FOUND)            \
    DEFINE_EFFECT(FONT_DIRECTORY_NOT_FOUND)     \
    DEFINE_EFFECT(VOICE_PACK_NOT_FOUND)         \
    DEFINE_EFFECT(ERROR_IN_BLADE_ARRAY)         \
    DEFINE_EFFECT(ERROR_IN_FONT_DIRECTORY)      \
    DEFINE_EFFECT(ERROR_IN_VOICE_PACK_VERSION)  \
    /* menu effects, triggered when you go from one menu entry to the next/previous */                             \
    DEFINE_EFFECT(MENU_CHANGE)                  \


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

#define DEFINE_EFFECT(X) case EFFECT_##X: return "EFFECT_" #X;
const char* EffectTypeName(EffectType effect) {
  switch (effect) {
    DEFINE_ALL_EFFECTS();
  }
  return "unknown effect";
}
#undef DEFINE_EFFECT

// Blade 0 is reserved for "other", which is displays, sound and anything that isn't a blade.
class BladeSet {
private:
#if NUM_BLADES < 8
  typedef uint8_t BitSetType;
#elif NUM_BLADES < 16
  typedef uint16_t BitSetType;
#elif NUM_BLADES < 32
  typedef uint32_t BitSetType;
#else
  typedef uint64_t BitSetType;
#endif
  constexpr BladeSet(BitSetType bits) : blades_(bits) {}

public:
  constexpr BladeSet() : blades_(0) {}
  static constexpr BladeSet fromBlade(int blade) { return BladeSet(1 << blade); }

  template<typename ... Args>
  static constexpr BladeSet fromBlade(int blade, Args... args) {
    return fromBlade(blade) | fromBlade(args...);
  }

  static constexpr BladeSet all() { return ~BladeSet(); }

  constexpr BladeSet operator|(const BladeSet& other) const {
    return BladeSet(blades_ | other.blades_);
  }
  constexpr BladeSet operator&(const BladeSet& other) const {
    return BladeSet(blades_ & other.blades_);
  }
  constexpr BladeSet operator~() const {
    return BladeSet(~blades_);
  }
  constexpr bool operator[](int blade) const {
    return (blades_ & (1 << blade)) != 0;
  }
  constexpr bool off() const {
    return blades_ == 0;
  }
  // Should this be if "other" is on, or if anything is on?
  // Right now it's if anything is on.
  constexpr bool on() const {
    return blades_ != 0;
  }

  constexpr bool operator==(const BladeSet& other) const { return blades_ == other.blades_; }
  constexpr bool operator!=(const BladeSet& other) const { return blades_ != other.blades_; }

  void operator|=(const BladeSet& other) {
    blades_ |= other.blades_;
  }
  void operator&=(const BladeSet& other) {
    blades_ &= other.blades_;
  }

  void printTo(Print& p) const {
    for (int i = 0; i < NUM_BLADES; i++) {
      if ((*this)[i]) {
        p.write((char)((i < 10 ? '0' : ('A'-10)) + i));
      }
    }
  }

private:
  BitSetType blades_;
};

class BladeBase;

class EffectLocation {
public:
#define DEFINE_BLADE_BITS(N) static constexpr BladeSet const BLADE##N = BladeSet::fromBlade(N);
#define DECLARE_BLADE_BITS(N) constexpr BladeSet const EffectLocation::BLADE##N;

  static constexpr BladeSet const ALL_BLADES = BladeSet::all();
  static constexpr BladeSet const MOST_BLADES = BladeSet::fromBlade(0);
  ONCEPERBLADE(DEFINE_BLADE_BITS);

  constexpr EffectLocation() : location_(0), blades_(BladeSet::all()) {}
  constexpr EffectLocation(uint16_t location, BladeSet blades) : location_(location), blades_(blades) {}
  constexpr EffectLocation(float f) :location_(f * 32768), blades_(BladeSet::all()) {}
  static EffectLocation rnd() {
    return EffectLocation(65535 + random(22937), BladeSet::all());
  }
  static EffectLocation rnd(BladeSet blades) {
    return EffectLocation(65535 + random(22937), blades);
  }
  BladeSet blades() const { return blades_; }
  bool on_blade(int bladenum) const { return blades_[bladenum]; }
  uint16_t fixed() const { return location_; }
  operator float() const { return fixed() / 32768.0; }
  void printTo(Print& p) const {
    p.print(fixed() / 32768.0);
    p.write('@');
    blades_.printTo(p);
  }
private:
  friend class SaberBase;
  uint16_t location_;
  BladeSet blades_;
};

struct BladeEffect {
  EffectType type;

  uint32_t start_micros;
  EffectLocation location; // 0 = base, 1 = tip
  float sound_length;
  int wavnum;
};


extern int GetBladeNumber(BladeBase *blade);

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

  static bool IsOn() {
    // Should this be if "other" is on, or if anything is on?
    // Right now it's if anything is on.
    return on_.on();
  }
  static bool BladeIsOn(int blade) { return on_[blade]; }
  static BladeSet OnBlades() { return on_; }
  static void TurnOn() {
    on_ = EffectLocation::ALL_BLADES;
    SaberBase::DoOn(0);
  }
  static void TurnOn(EffectLocation location) {
    PVLOG_DEBUG << "TurnOn " << location << "\n";
    // You can't turn on a blade that's already on.
    location.blades_ &=~ on_;
    PVLOG_DEBUG << "TurnOn2 " << location << "\n";
    if (location.blades_.off()) return;
    PVLOG_DEBUG << "TurnOn3 " << location << "\n";
    on_ |= location.blades();
    SaberBase::DoOn(location);
  }
  static void TurnOff(OffType off_type) {
    on_ = BladeSet();
    last_motion_request_ = millis();
    SaberBase::DoOff(off_type, 0);
  }
  static void TurnOff(OffType off_type, EffectLocation location) {
    PVLOG_DEBUG << "TurnOff " << location << "\n";
    location.blades_ &= on_; // can only turn off blades which are on
    PVLOG_DEBUG << "TurnOff " << location << "\n";
    if (location.blades_.off()) return;
    PVLOG_DEBUG << "TurnOff " << location << "\n";
    on_ &=~ location.blades();
    last_motion_request_ = millis();
    SaberBase::DoOff(off_type, location);
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
    LOCKUP_ARMED,            // For detonators and such
    LOCKUP_AUTOFIRE,         // For blasters and phasers
    LOCKUP_MELT,             // For cutting through doors...
    LOCKUP_LIGHTNING_BLOCK,  // Lightning block lockup
  };
  static LockupType lockup_;
  static BladeSet lockup_blades_;
  static LockupType Lockup() { return lockup_; }
  static LockupType LockupForBlade(int blade) {
    if (!lockup_blades_[blade]) return LOCKUP_NONE;
    return lockup_;
  }
  static void SetLockup(LockupType lockup, BladeSet blades = BladeSet::all()) {
    lockup_ = lockup;
    lockup_blades_ = blades;
  }

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

  virtual bool CheckBlade(EffectLocation location) { return true; }
  static EffectLocation location; // fallback

#define SABERFUN(NAME, TYPED_ARGS, ARGS)                        \
private:                                                        \
  static void Do##NAME##Internal TYPED_ARGS {                   \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {    \
      p->SB_##NAME ARGS;                                        \
    }                                                           \
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {    \
      if (p->CheckBlade(location))  p->SB_##NAME##2 ARGS;       \
    }                                                           \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
  }                                                             \
                                                                \
public:                                                         \
  virtual void SB_##NAME TYPED_ARGS {}                          \
  virtual void SB_##NAME##2 TYPED_ARGS {}

#define SABERBASEFUNCTIONS()                                                            \
  SABERFUN(Effect, (EffectType effect, EffectLocation location), (effect, location));   \
  SABERFUN(On, (EffectLocation location), (location));                                  \
  SABERFUN(Off, (OffType off_type, EffectLocation location), (off_type, location));     \
  SABERFUN(Change, (ChangeType change_type), (change_type));                            \
                                                                                        \
  SABERFUN(Top, (uint64_t total_cycles), (total_cycles));                               \
  SABERFUN(IsOn, (bool* on), (on));

  SABERBASEFUNCTIONS();


private:
  static void DoEffectInternal2(EffectType effect, EffectLocation location) {
    DoEffectInternal(effect, location);
    PushEffect(effect, location);
  }
public:
  static void DoEffect(EffectType effect, EffectLocation location) {
    ClearSoundInfo();
    DoEffectInternal2(effect, location);
  }
  static void DoOn(EffectLocation location) {
    ClearSoundInfo();
    DoOnInternal(location);
    DoEffectInternal2(EFFECT_IGNITION, location);
  }
  static void DoOff(OffType off_type, EffectLocation location) {
    ClearSoundInfo();
    DoOffInternal(off_type, location);
    switch (off_type) {
      case OFF_BLAST:
        DoEffectInternal2(EFFECT_BLAST, location);
        break;
      case OFF_NORMAL:
      case OFF_FAST:
        DoEffectInternal2(EFFECT_RETRACTION, location);
        break;
      case OFF_IDLE:
      case OFF_CANCEL_PREON:
        // do nothing
        break;
    }

  }
  static void DoBladeDetect(bool detected) {
    ClearSoundInfo();
    if (detected) {
      DoEffect(EFFECT_BLADEIN, 0);
    } else {
      DoEffect(EFFECT_BLADEOUT, 0);
    }
  }
  static void DoChange(ChangeType change_type) {
    ClearSoundInfo();
    DoChangeInternal(change_type);
  }
  static void DoTop(uint64_t total_cycles) {
    ClearSoundInfo();
    DoTopInternal(total_cycles);
  }
  static void DoIsOn(bool* on) {
    ClearSoundInfo();
    DoIsOnInternal(on);
  }

  static void DoEffect(EffectType e, EffectLocation location, int N) {
    sound_length = 0.0;
    sound_number = N;
    DoEffectInternal2(e, location);
  }
  static void DoEffectR(EffectType e) { DoEffect(e, EffectLocation::rnd()); }
  static void DoBlast() { DoEffectR(EFFECT_BLAST); }
  static void DoForce() { DoEffectR(EFFECT_FORCE); }
  static void DoBoot() { DoEffect(EFFECT_BOOT, 0); }
  static void DoPreOn() { DoEffect(EFFECT_PREON, 0); }
  static void DoPreOn(EffectLocation l) { DoEffect(EFFECT_PREON, l); }
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

  static size_t GetEffects(BladeEffect** blade_effects) {
    *blade_effects = effects_;
    while (num_effects_ &&
           micros() - effects_[num_effects_-1].start_micros > 7000000) {
      num_effects_--;
    }
    return num_effects_;
  }


  // Not private for debugging purposes only.
  static uint32_t last_motion_request_;

private:
  static void PushEffect(EffectType type, EffectLocation location) {
    switch (type) {
      default: break;

      // Clear out all old effects when we go to a new preset.
      case EFFECT_CHDIR:
        num_effects_ = 0;
        break;

      case EFFECT_LOCKUP_BEGIN:
        switch (SaberBase::Lockup()) {
          case LOCKUP_DRAG:
            type = EFFECT_DRAG_BEGIN;
          case LOCKUP_NORMAL:
            break;
          default: return;
        }
        break;
      case EFFECT_LOCKUP_END:
        switch (SaberBase::Lockup()) {
          case LOCKUP_DRAG:
            type = EFFECT_DRAG_END;
          case LOCKUP_NORMAL:
            break;
          default: return;
        }
        break;
      case EFFECT_CLASH_UPDATE:
        // Not stored in queue
        return;
    }
    for (size_t i = std::min(num_effects_, NELEM(effects_) - 1); i; i--) {
      effects_[i] = effects_[i-1];
    }
    effects_[0].type = type;
    effects_[0].start_micros = micros();
    effects_[0].location = location;
    effects_[0].sound_length = sound_length;
    effects_[0].wavnum = sound_number;
    num_effects_ = std::min(num_effects_ + 1, NELEM(effects_));
  }


  static size_t num_effects_;
  static BladeEffect effects_[10];
  static BladeSet on_;
  static uint32_t current_variation_;
  static ColorChangeMode color_change_mode_;
  SaberBase* next_saber_;
};

size_t SaberBase::num_effects_ = 0;
BladeEffect SaberBase::effects_[10];
EffectLocation SaberBase::location;
BladeSet SaberBase::on_ = BladeSet();
BladeSet SaberBase::lockup_blades_ = BladeSet();

constexpr BladeSet const EffectLocation::ALL_BLADES;
constexpr BladeSet const EffectLocation::MOST_BLADES;
ONCEPERBLADE(DECLARE_BLADE_BITS);

#endif
