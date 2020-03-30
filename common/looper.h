#ifndef COMMON_LOOPER_H
#define COMMON_LOOPER_H

// Helper class for classses that needs to be called back from the Loop()
// function. Also provides a Setup() function.
class Looper;
Looper* loopers = NULL;
Looper* hf_loopers = NULL;
LoopCounter global_loop_counter;
class Looper {
public:
  void Link() {
    CHECK_LL(Looper, loopers, next_looper_);
    next_looper_ = loopers;
    loopers = this;
    CHECK_LL(Looper, loopers, next_looper_);
  }
  // High-Frequency Loopers go at the end of the list
  void HighFrequencyLink() {
    CHECK_LL(Looper, loopers, next_looper_);
    Looper** i = &loopers;
    while (*i) i = &(*i)->next_looper_;
    next_looper_ = nullptr;
    *i = this;
    if (!hf_loopers) hf_loopers = this;
    CHECK_LL(Looper, loopers, next_looper_);
  }
  void Unlink() {
    CHECK_LL(Looper, loopers, next_looper_);
    if (hf_loopers == this) hf_loopers = next_looper_;
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
  explicit Looper(HFLink _) { HighFrequencyLink(); }
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
  static void DoHFLoop() {
    ScopedCycleCounter cc(loop_cycles);
    CHECK_LL(Looper, loopers, next_looper_);
    for (Looper *l = hf_loopers; l; l = l->next_looper_) {
      // TODO: We're currently double-counting these cycles, since
      // DoHfLoop() is likely to be called from inside of DoLoop()
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
  static void LoopTop(float total_cycles) {
    for (Looper *l = loopers; l; l = l->next_looper_) {
      STDOUT.print(l->name());
      STDOUT.print(" loop: ");
      STDOUT.print(l->cycles_ * 100.0f / total_cycles);
      STDOUT.println("%");
      l->cycles_ = 0;
    }
    loop_cycles = 0;
  }
  static uint64_t CountCycles() {
    uint64_t cycles = loop_cycles;
    for (Looper *l = loopers; l; l = l->next_looper_)
      cycles += l->cycles_;
    return cycles;
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
