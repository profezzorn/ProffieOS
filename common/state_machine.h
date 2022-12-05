#ifndef COMMON_STATE_MACHINE_H
#define COMMON_STATE_MACHINE_H

/*
 * State machine code.
 * This code uses the fact that switch can jump into code to create
 * lightweight pseudo-thread. These threads have a lot of limitations,
 * but they can make code much easier to read compared to manually
 * written state machines. Lots of examples below.
 */
// Note, you cannot have two YIELD() on the same line.
#define YIELD() do { state_machine_.next_state_ = __LINE__; return; case __LINE__: break; } while(0)
#define SLEEP(MILLIS) do { state_machine_.sleep_until_ = millis() + (MILLIS); while (millis() < state_machine_.sleep_until_) YIELD(); } while(0)
#define SLEEP_MICROS(MICROS) do { state_machine_.sleep_until_ = micros() + (MICROS); while (micros() < state_machine_.sleep_until_) YIELD(); } while(0)
#define STATE_MACHINE_BEGIN() switch(state_machine_.next_state_) { case -1:
#define STATE_MACHINE_END() state_machine_.next_state_ = -2; case -2: break; } 

#define CALL(SM, FUN) do {                  \
  state_machine_.next_state_ = __LINE__;    \
  case __LINE__:                            \
  FUN();                                    \
  if (SM.next_state == -2) return;          \
  SM.reset_state_machine();                 \
} while(0)

struct StateMachineState {
  int next_state_ = -1;
  uint32_t sleep_until_ = 0;
  void reset_state_machine() {
    next_state_ = -1;
  }
  bool done() const { return next_state_ == -2; }
};

class StateMachine {
protected:
  StateMachineState state_machine_;
};

#endif
