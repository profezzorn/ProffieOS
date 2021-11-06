#ifndef SCRIPTS_TEST_MOTION_TIMEOUT_H
#define SCRIPTS_TEST_MOTION_TIMEOUT_H

class MotionTimeoutScript : Looper, StateMachine {
public:
  const char* name() override { return "TestMotionTimeoutScript"; }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    SLEEP(5000);
    while(1) {
      SaberBase::last_motion_request_ = millis() - 100000; // 100 seconds ago, should trigger timeout
      SLEEP(1000); // Motion should be timed out.
      SaberBase::RequestMotion(); // Start up motion again.
      SLEEP(1000);
    }
    STATE_MACHINE_END();
  }
};

#endif
