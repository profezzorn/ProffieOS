#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string.h>

#define DESCRIBE(X) case X: return #X


// cruft
#define NELEM(X) (sizeof(X)/sizeof((X)[0]))
#define SCOPED_PROFILER() do { } while(0)
template<class A, class B>
constexpr auto min(A&& a, B&& b) -> decltype(a < b ? std::forward<A>(a) : std::forward<B>(b)) {
  return a < b ? std::forward<A>(a) : std::forward<B>(b);
}
template<class A, class B>
constexpr auto max(A&& a, B&& b) -> decltype(a < b ? std::forward<A>(a) : std::forward<B>(b)) {
  return a >= b ? std::forward<A>(a) : std::forward<B>(b);
}
float fract(float x) { return x - floor(x); }

uint32_t micros_ = 0;
uint32_t micros() { return micros_; }
uint32_t millis() { return micros_ / 1000; }
int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}

int random(int x) { return (random(x) & 0x7fffff) % x; }

template<class T, class U>
struct is_same_type { static const bool value = false; };
 
template<class T>
struct is_same_type<T, T> { static const bool value = true; };


// This really ought to be a typedef, but it causes problems I don't understand.
#define StyleAllocator class StyleFactory*

struct SaberBase {
  static uint32_t GetCurrentVariation() {
    return 0;
  }
};

#define HEX 16

struct  Print {
  void print(const char* s) { puts(s); }
  void print(int v, int base) { fprintf(stdout, "%d", v); }
  void print(float v) { fprintf(stdout, "%f", v); }
  void write(char s) { putchar(s); }
  template<class T>
  void println(T s) { print(s); putchar('\n'); }
};

template<typename T, typename X = void> struct PrintHelper {
  static void out(Print& p, T& x) { p.print(x); }
};

template<typename T> struct PrintHelper<T, decltype(((T*)0)->printTo(*(Print*)0))> {
  static void out(Print& p, T& x) { x.printTo(p); }
};

struct ConsoleHelper : public Print {
  template<typename T, typename Enable = void>
  ConsoleHelper& operator<<(T v) {
    PrintHelper<T>::out(*this, v);
    return *this;
  }
};

ConsoleHelper STDOUT;

uint64_t loop_cycles = 0;

class ScopedCycleCounter {
public:
  ScopedCycleCounter(uint64_t& dest) {}
};

#include "../common/events.h"
#include "../common/linked_list.h"
#include "../common/loop_counter.h"
#include "../common/looper.h"
#include "../common/state_machine.h"

const char* DescribeEvent(EVENT e) {
  switch (e) {
    DESCRIBE(EVENT_NONE);
    DESCRIBE(EVENT_PRESSED);
    DESCRIBE(EVENT_RELEASED);
    DESCRIBE(EVENT_HELD);
    DESCRIBE(EVENT_HELD_MEDIUM);
    DESCRIBE(EVENT_HELD_LONG);
    DESCRIBE(EVENT_CLICK_SHORT);
    DESCRIBE(EVENT_SAVED_CLICK_SHORT);
    DESCRIBE(EVENT_CLICK_LONG);
    DESCRIBE(EVENT_FIRST_PRESSED);
    DESCRIBE(EVENT_FIRST_RELEASED);
    DESCRIBE(EVENT_FIRST_HELD);
    DESCRIBE(EVENT_FIRST_HELD_MEDIUM);
    DESCRIBE(EVENT_FIRST_HELD_LONG);
    DESCRIBE(EVENT_FIRST_CLICK_SHORT);
    DESCRIBE(EVENT_FIRST_SAVED_CLICK_SHORT);
    DESCRIBE(EVENT_FIRST_CLICK_LONG);
    DESCRIBE(EVENT_SECOND_PRESSED);
    DESCRIBE(EVENT_SECOND_RELEASED);
    DESCRIBE(EVENT_SECOND_HELD);
    DESCRIBE(EVENT_SECOND_HELD_MEDIUM);
    DESCRIBE(EVENT_SECOND_HELD_LONG);
    DESCRIBE(EVENT_SECOND_CLICK_SHORT);
    DESCRIBE(EVENT_SECOND_SAVED_CLICK_SHORT);
    DESCRIBE(EVENT_SECOND_CLICK_LONG);
    DESCRIBE(EVENT_THIRD_PRESSED);
    DESCRIBE(EVENT_THIRD_RELEASED);
    DESCRIBE(EVENT_THIRD_HELD);
    DESCRIBE(EVENT_THIRD_HELD_MEDIUM);
    DESCRIBE(EVENT_THIRD_HELD_LONG);
    DESCRIBE(EVENT_THIRD_CLICK_SHORT);
    DESCRIBE(EVENT_THIRD_SAVED_CLICK_SHORT);
    DESCRIBE(EVENT_THIRD_CLICK_LONG);
    DESCRIBE(EVENT_FOURTH_PRESSED);
    DESCRIBE(EVENT_FOURTH_RELEASED);
    DESCRIBE(EVENT_FOURTH_HELD);
    DESCRIBE(EVENT_FOURTH_HELD_MEDIUM);
    DESCRIBE(EVENT_FOURTH_HELD_LONG);
    DESCRIBE(EVENT_FOURTH_CLICK_SHORT);
    DESCRIBE(EVENT_FOURTH_SAVED_CLICK_SHORT);
    DESCRIBE(EVENT_FOURTH_CLICK_LONG);
    DESCRIBE(EVENT_LATCH_ON);
    DESCRIBE(EVENT_LATCH_OFF);
    DESCRIBE(EVENT_STAB);
    DESCRIBE(EVENT_SWING);
    DESCRIBE(EVENT_SHAKE);
    DESCRIBE(EVENT_TWIST);
    DESCRIBE(EVENT_CLASH);
  }
  return "unknown event";
}

BUTTON last_button_ = BUTTON_NONE;
EVENT last_event_ = EVENT_NONE;
bool accept_event_ = false;

class TestHelper;
TestHelper* all_tests_ = NULL;
TestHelper* current_test_ = NULL;
class TestHelper : public StateMachine {
public:
  TestHelper* next_;
  TestHelper() {
    next_ = all_tests_;
    all_tests_ = this;
  }
  bool done() { return state_machine_.next_state_ == -2; }
  virtual const char* name() = 0;
  void Test() {
    ActualTest();
  }

  virtual void ActualTest() = 0;

  bool pass_ = true;
};

class TestProp {
public:
  virtual bool Event(enum BUTTON button, EVENT event) {
    last_button_ = button;
    last_event_ = event;
    current_test_->Test();

    if (accept_event_) {
      accept_event_ = false;

      // current_modifiers &= button;
      current_modifiers = 0;
      return true;
    } else {
      return false;
    }
  }
};

TestProp prop;


#include "../common/command_parser.h"
#include "button_base.h"

CommandParser* parsers = NULL;

bool on_ = true;

class TestButton : public ButtonBase {
public:
  TestButton(enum BUTTON button, const char* name) : ButtonBase(name, button) {}
  bool pressed_ = false;
  bool Read() override { return pressed_; }
};

#define EXPECT_EQ(X,Y) do {					\
  auto x = (X);							\
  auto y = (Y);							\
  if (x != y) {							\
    std::cerr << #X << " != " << #Y << " on line " << __LINE__ << std::endl; \
    std::cerr << #X << " = " << x  << std::endl;				\
    std:: cerr << #Y << " = " << y << std::endl;				\
    exit(1);							\
  }								\
} while(0)

#define EXPECT_EQ_EVENT(X,Y) do {					\
  auto x = (X);							\
  auto y = (Y);							\
  if (x != y) {							\
    std::cerr << #X << " != " << #Y << " on line " << __LINE__ << std::endl; \
    std::cerr << #X << " = " << DescribeEvent(x) << "(" << x << ")"  << std::endl; \
    std::cerr << #Y << " = " << DescribeEvent(y) << "(" << y << ")"  << std::endl; \
    exit(1);							\
  }								\
} while(0)

#define TEST(X) class X : public TestHelper {	\
public:						\
  TestHelper* next_;				\
  TestButton pow;                               \
  TestButton aux; 			\
  TestButton aux2; 			\
  uint32_t start;                               \
  uint32_t target_time_ = 0;      \
  const char* name() override { return #X; }    \
  X() : pow(BUTTON_POWER, "pow")		\
	 , aux(BUTTON_AUX, "aux") 	\
	 , aux2(BUTTON_AUX2, "aux2") 	{	\
  }						\
  void ActualTest() override;			\
};						\
X X##_test;					\
void X::ActualTest()

#define EXPECT_EVENT(B, X) do {						\
  start = micros_;							\
  last_button_ = BUTTON_NONE;						\
  last_event_ = EVENT_NONE;						\
  while (last_button_ == BUTTON_NONE) {					\
    YIELD();								\
    if (micros_ - start > 2000000) break;				\
  }									\
  if (micros_ - start > 20000) {					\
    pass_ = false;							\
    fprintf(stderr, "Took too long! @%d millis=%d start=%d\n", __LINE__, (micros_ - start)/1000, start/1000); \
    break;								\
  }									\
  EXPECT_EQ(last_button_, (B));						\
  EXPECT_EQ_EVENT(last_event_, (X));						\
  last_button_ = BUTTON_NONE;						\
  last_event_ = EVENT_NONE;						\
  /* fprintf(stderr, "Expectation met @%d\n", __LINE__); */		\
} while(0)
    
#define ACCEPT_EVENT(B, X) do {			\
  EXPECT_EVENT(B, X);				\
  accept_event_ = true;				\
} while(0)

#undef SLEEP  
#define SLEEP(X) do {							\
  start = target_time_;   \
  target_time_ += (X) * 1000; \
  while (micros_ < target_time_) {				\
    YIELD();								\
    if (last_button_ != BUTTON_NONE) {					\
      fprintf(stderr, "Got event while sleeping @%d: %d  %s (%d) t = %d\n", __LINE__, last_button_, DescribeEvent(last_event_), last_event_, (micros_ - start)/1000); \
      last_button_ = BUTTON_NONE;					\
      last_event_ = EVENT_NONE;						\
      pass_ = false;							\
      break;								\
    }									\
  }  \
} while(0)

TEST(LongClickTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD);
  SLEEP(500);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD_MEDIUM);
  SLEEP(1200);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_LONG);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD_LONG);
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_CLICK_LONG);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_LONG);
  STATE_MACHINE_END();
}

TEST(LongHeldTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD);
  SLEEP(500);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD_MEDIUM);
  SLEEP(1200);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_LONG);
  ACCEPT_EVENT(BUTTON_POWER, EVENT_HELD_LONG);
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  STATE_MACHINE_END();
}

TEST(ShortClickTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_SAVED_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SAVED_CLICK_SHORT);
  STATE_MACHINE_END();
}

TEST(DoubleClickTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(200);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_SAVED_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SAVED_CLICK_SHORT);
  STATE_MACHINE_END();
}

TEST(TripleClickTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(200);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(200);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_THIRD_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_THIRD_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_THIRD_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_THIRD_SAVED_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SAVED_CLICK_SHORT);
  STATE_MACHINE_END();
}

TEST(QuadClickTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(200);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SECOND_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);

  SLEEP(200);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_THIRD_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_THIRD_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_THIRD_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(200);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FOURTH_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(200); 
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FOURTH_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FOURTH_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_SHORT);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FOURTH_SAVED_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_POWER, EVENT_SAVED_CLICK_SHORT);
  STATE_MACHINE_END();
}

TEST(PressReleaseTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  ACCEPT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD);
  SLEEP(500);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD_MEDIUM);
  SLEEP(1200);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_LONG);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD_LONG);
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  ACCEPT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  STATE_MACHINE_END();
}

// In this two-button test, no events are accepted, so all events come through.
TEST(TwoButtonClickTest) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD);
  SLEEP(100);
  aux.pressed_ = true;
  EXPECT_EVENT(BUTTON_AUX, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_AUX, EVENT_PRESSED);
  SLEEP(100);
  aux.pressed_ = false;
  EXPECT_EVENT(BUTTON_AUX, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_AUX, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_AUX, EVENT_CLICK_SHORT);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_MEDIUM);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD_MEDIUM);
  SLEEP(100);
  EXPECT_EVENT(BUTTON_AUX, EVENT_FIRST_SAVED_CLICK_SHORT);
  EXPECT_EVENT(BUTTON_AUX, EVENT_SAVED_CLICK_SHORT);
  SLEEP(1100);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD_LONG);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD_LONG);
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_CLICK_LONG);
  EXPECT_EVENT(BUTTON_POWER, EVENT_CLICK_LONG);
  STATE_MACHINE_END();
}

// In this two-button test, no events are accepted, so all events come through.
TEST(TwoButtonClickTest2) {
  STATE_MACHINE_BEGIN();
  SLEEP(10000);
  pow.pressed_ = true;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_PRESSED);
  SLEEP(300);
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_HELD);
  EXPECT_EVENT(BUTTON_POWER, EVENT_HELD);
  SLEEP(100);
  aux.pressed_ = true;
  EXPECT_EVENT(BUTTON_AUX, EVENT_FIRST_PRESSED);
  EXPECT_EVENT(BUTTON_AUX, EVENT_PRESSED);
  SLEEP(100);
  aux.pressed_ = false;
  EXPECT_EVENT(BUTTON_AUX, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_AUX, EVENT_RELEASED);
  EXPECT_EVENT(BUTTON_AUX, EVENT_FIRST_CLICK_SHORT);
  ACCEPT_EVENT(BUTTON_AUX, EVENT_CLICK_SHORT);
  SLEEP(300);
  SLEEP(100);
  SLEEP(1100);
  pow.pressed_ = false;
  EXPECT_EVENT(BUTTON_POWER, EVENT_FIRST_RELEASED);
  EXPECT_EVENT(BUTTON_POWER, EVENT_RELEASED);
  STATE_MACHINE_END();
}

int main() {
  bool pass = true;
  for (current_test_ = all_tests_; current_test_; current_test_ = current_test_->next_) {
    fprintf(stderr, "Running %s\n", current_test_->name());
    micros_ = 0;
    last_event_ = EVENT_NONE;
    last_button_ = BUTTON_NONE;
    current_test_->Test();
    while (!current_test_->done()) {
      micros_ += 100;
      Looper::DoLoop();
      current_test_->Test();
    }
    // Make sure that we don't generate any more events after the test.
    // If so, crash...
    for (int i = 0; i < 10000; i++) {
      micros_ += 100;
      Looper::DoLoop();
      if (last_button_ != BUTTON_NONE) {
        fprintf(stderr, "Got event after test: %d  %s (%d)\n", last_button_, DescribeEvent(last_event_), last_event_);
	pass = false;
	break;
      }
    }
    if (!current_test_->pass_) pass = false;
  }
  exit(pass ? 0 : 1);
}
