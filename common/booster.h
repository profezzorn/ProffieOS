#ifndef COMMON_BOOSTER_H
#define COMMON_BOOSTER_H

#if VERSION_MAJOR >= 4

// Turns off booster when we don't need it anymore.
class Booster : Looper, StateMachine, CommandParser {
public:
  Booster() : Looper(), CommandParser() {}
  const char* name() override { return "Booster"; }

  bool Active() {
    if (amplifier.Active()) return true;
    uint32_t t = millis() - last_enabled_;
    if (t < 10000) return true;
    if (saber.NeedsPower()) return true;
    bool on = false;
    SaberBase::DoIsOn(&on);
    return on;
  }

  void Enable() {
    last_enabled_ = millis();
    if (!digitalRead(boosterPin)) {
      digitalWrite(boosterPin, HIGH);
      delay(10); // Give it a little time to wake up.
    }
  }

protected:
  void Setup() override {
    pinMode(boosterPin, OUTPUT);
    last_enabled_ = millis();
  }

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (true) {
      while (Active()) YIELD();
      // 100 * 0.1 s = 10 seconds
      for (i_ = 0; i_ < 100 && !Active(); i_++)
	SLEEP(100);
      if (Active()) continue;
      STDOUT.println("Booster off.");
      digitalWrite(boosterPin, LOW); // turn the booster off
      while (!Active()) YIELD();
    }
    STATE_MACHINE_END();
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "booster")) {
      if (!strcmp(arg, "on")) {
        digitalWrite(boosterPin, HIGH); // turn the booster off
        return true;
      }
      if (!strcmp(arg, "off")) {
        digitalWrite(boosterPin, LOW); // turn the booster off
        return true;
      }
    }
    return false;
  }

  void Help() {
    STDOUT.println(" booster on/off - turn booster on or off");
  }

private:
  int i_;
  uint32_t last_enabled_;
};

Booster booster;

void EnableBooster() {
  booster.Enable();
}

#else
void EnableBooster() { }
#endif   // V4
#endif
