#ifndef COMMON_LOOPER_H
#define COMMON_LOOPER_H

// Helper class for classses that needs to be called back from the Loop()
// function. Also provides a Setup() function.
class Looper;
Looper* loopers = NULL;
LoopCounter global_loop_counter;
class Looper {
public:
  void Link() {
    CHECK_LL(Looper, loopers, next_looper_);
    next_looper_ = loopers;
    loopers = this;
    CHECK_LL(Looper, loopers, next_looper_);
  }
  void Unlink() {
    CHECK_LL(Looper, loopers, next_looper_);
    for (Looper** i = &loopers; *i; i = &(*i)->next_looper_) {
      if (*i == this) {
        *i = next_looper_;
        CHECK_LL(Looper, loopers, next_looper_);
        return;
      }
    }
    CHECK_LL(Looper, loopers, next_looper_);
  }
  Looper() { Link(); }
  explicit Looper(NoLink _) { }
  ~Looper() { Unlink(); }
  static void DoLoop() {
    ScopedCycleCounter cc(loop_cycles);
    CHECK_LL(Looper, loopers, next_looper_);
    for (Looper *l = loopers; l; l = l->next_looper_) {
      ScopedCycleCounter cc2(l->cycles_);
      l->Loop();
    }
    global_loop_counter.Update();
  }
  static void DoSetup() {
    for (Looper *l = loopers; l; l = l->next_looper_) {
      l->Setup();
    }
  }
  static void LoopTop(double total_cycles) {
    for (Looper *l = loopers; l; l = l->next_looper_) {
      STDOUT.print(l->name());
      STDOUT.print(" loop: ");
      STDOUT.print(l->cycles_ * 100.0 / total_cycles);
      STDOUT.println("%");
      l->cycles_ = 0;
    }
  }

protected:
  virtual const char* name() = 0;
  virtual void Loop() = 0;
  virtual void Setup() {}
private:
  uint64_t cycles_ = 0;
  Looper* next_looper_;
};

#endif
