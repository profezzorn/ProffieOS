#ifndef FUNCTIONS_SEQUENCE_H
#define FUNCTIONS_SEQUENCE_H

template<int millis_per_bit, int bits, int ... sequence >
class SequenceF {
public:
  void run(BladeBase* blade) {
    static uint16_t sequence_[] = { sequence... };
    uint32_t now = millis();
    uint32_t bit = (now / millis_per_bit) % std::min<size_t>(bits, sizeof...(sequence) * 16);
    on_ = !!((sequence_[bit >> 4] >> ((~bit) & 0xf)) & 1);
  }
  int getColor(int led) { return on_ * 32768; }
  
private:
  bool on_;
};

#endif
