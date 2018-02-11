#ifndef COMMON_SABER_BASE_H
#define COMMON_SABER_BASE_H

// SaberBase is our main class for distributing saber-related events, such
// as on/off/clash/etc. to where they need to go. Each SABERFUN below
// has a corresponding SaberBase::Do* function which invokes that function
// on all active SaberBases.
class SaberBase;
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
  enum LockupType {
    LOCKUP_NONE,
    LOCKUP_NORMAL,
    LOCKUP_DRAG,
  };
  static LockupType Lockup() { return lockup_; }
  static void SetLockup(LockupType lockup) { lockup_ = lockup; }

  struct Blast {
    uint32_t start_micros;
    float location; // 0 = base, 1 = tip
  };

  static size_t NumBlasts() {
    while (num_blasts_ &&
           micros() - blasts_[num_blasts_-1].start_micros > 5000000) {
      num_blasts_--;
    }
    return num_blasts_;
  }
  static const Blast& getBlast(size_t i) {
    return blasts_[i];
  }
  static void addBlast(float location) {
    for (size_t i = NELEM(blasts_) - 1; i; i--) {
      blasts_[i] = blasts_[i-1];
    }
    blasts_[0].start_micros = micros();
    blasts_[0].location = location;
    num_blasts_ = min(num_blasts_ + 1, NELEM(blasts_));
  }

  // 1.0 = kDefaultVolume
  virtual void SetHumVolume(float volume) {}

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
  SABERFUN(Speedup, (), ());                    \
                                                \
  /* Swing rotation speed degrees per second */ \
  SABERFUN(Motion, (const Vec3& gyro), (gyro)); \
  /* Accelertation in g */                      \
  SABERFUN(Accel, (const Vec3& accel), (accel));\
                                                \
  SABERFUN(Top, (), ());                        \
  SABERFUN(IsOn, (bool* on), (on));             \
  SABERFUN(Message, (const char* msg), (msg));

  SABERBASEFUNCTIONS();
#undef SABERFUN

private:
  static size_t num_blasts_;
  static struct Blast blasts_[3];
  static LockupType lockup_;
  SaberBase* next_saber_;
};

#endif
