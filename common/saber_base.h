#ifndef COMMON_SABER_BASE_H
#define COMMON_SABER_BASE_H

// SaberBase is our main class for distributing saber-related events, such
// as on/off/clash/etc. to where they need to go. Each SABERFUN below
// has a corresponding SaberBase::Do* function which invokes that function
// on all active SaberBases.
class SaberBase;
class Effect;
extern SaberBase* saberbases;

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
  static bool IsOn() { return on_; }
  static void TurnOn() {
    on_ = true;
    SaberBase::DoOn();
  }
  static void TurnOff() {
    on_ = false;
    last_motion_request_ = millis();
    SaberBase::DoOff();
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

  enum LockupType {
    LOCKUP_NONE,
    LOCKUP_NORMAL,
    LOCKUP_DRAG,
  };
  static LockupType Lockup() { return lockup_; }
  static void SetLockup(LockupType lockup) { lockup_ = lockup; }

  // 1.0 = kDefaultVolume
  // This is really just for sound fonts.
  virtual void SetHumVolume(float volume) {}
  virtual void StartSwing() {}
  virtual float SetSwingVolume(float swing_strength, float AccentSwingVolumeSharpness,
  float MaxAccentSwingVolume, float MaxAccentSwingDucking, float mixhum) {}
  virtual bool IsSwingPlaying() {}
  
#define SABERFUN(NAME, TYPED_ARGS, ARGS)                        \
public:                                                         \
  static void Do##NAME TYPED_ARGS {                             \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
    for (SaberBase *p = saberbases; p; p = p->next_saber_) {    \
      p->SB_##NAME ARGS;                                        \
    }                                                           \
    CHECK_LL(SaberBase, saberbases, next_saber_);               \
  }                                                             \
                                                                \
  virtual void SB_##NAME TYPED_ARGS {}

#define SABERBASEFUNCTIONS()                    \
  SABERFUN(Clash, (), ());                      \
  SABERFUN(Stab, (), ());                       \
  SABERFUN(On, (), ());                         \
  SABERFUN(Off, (), ());                        \
  SABERFUN(Force, (), ());                      \
  SABERFUN(Blast, (), ());                      \
  SABERFUN(Boot, (), ());                       \
  SABERFUN(NewFont, (), ());                    \
  SABERFUN(BeginLockup, (), ());                \
  SABERFUN(EndLockup, (), ());                  \
                                                \
  SABERFUN(Top, (), ());                        \
  SABERFUN(Relax, (), ());                      \
  SABERFUN(IsOn, (bool* on), (on));             \
  SABERFUN(Message, (const char* msg), (msg));

  SABERBASEFUNCTIONS();
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

private:
  static bool on_;
  static LockupType lockup_;
  static uint32_t last_motion_request_;
  SaberBase* next_saber_;
};

#endif
