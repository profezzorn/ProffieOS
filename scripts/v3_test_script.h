#ifndef SCRIPTS_V3_TEST_SCRIPT_H
#define SCRIPTS_V3_TEST_SCRIPT_H

#ifdef TEENSYDUINO

class V3TestScript : Looper, StateMachine {
public:
  const char* name() override { return "Script"; }
  void Loop() override {
    STATE_MACHINE_BEGIN();
    SLEEP(2000);
    if (fabsf(prop.id() - 125812.5f) > 22687.0f) {
      STDOUT.println("ID IS WRONG!!!");
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
      beeper.Beep(0.5, 2000.0);
      SLEEP(1000);
    }
    CommandParser::DoParse("on", NULL);
    SLEEP(1000);
    CommandParser::DoParse("batt", NULL);
    SLEEP(2000);
    CommandParser::DoParse("play", "cantina.wav");
#if 0    
    while (true) {
      if (dac.isSilent()) {
        SLEEP(2000);
      } else {
        CommandParser::DoParse("clash", NULL);
        STDOUT.print("alloced: ");
        STDOUT.println(mallinfo().uordblks);
        SLEEP(100);
      }
    }
#endif    
    STATE_MACHINE_END();
  }
  void Run() {
    state_machine_.reset_state_machine();
    run_ = true;
  }
  bool run_ = false;
};

#endif

#endif
