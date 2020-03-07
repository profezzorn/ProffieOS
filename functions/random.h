#ifndef FUNCTIONS_RANDOM_H
#define FUNCTIONS_RANDOM_H

class RandomF {
public:
  void run(BladeBase* blade) { mix_ = random(32768); }
  int getInteger(int led) { return mix_; }
private:
  int mix_;
};

class RandomPerLEDF {
public:
  void run(BladeBase* blade) {  }
  int getInteger(int led) { return random(32768); }
};


#endif
