#ifndef FUNCTIONS_SEQUENCE_H
#define FUNCTIONS_SEQUENCE_H

// usage: SequenceF<millis_per_bits, bits, 0b0000000000000000, ....>
// millis_per_bit: a number, millseconds spent on each bit
// bits: a number, number of bits before we loop around to the beginning
// 0b0000000000000000: 16-bit binary numbers containing the actual sequence.
//
// Returns 32768 if the current bit in the sequence is 1, 0 otherwise.
// The number of 16-bit binary numbers should be at least |bits| / 16, rounded up.
// Note that if not all bits are used within the 16-bit number.
// Example, an SOS pattern:
// SequenceF<100, 37, 0b0001010100011100, 0b0111000111000101, 0b0100000000000000>

template<int millis_per_bit, int bits, int ... sequence >
class SequenceF {
public:
  void run(BladeBase* blade) {
    static uint16_t sequence_[] = { sequence... };
    uint32_t now = millis();
    uint32_t bit = (now / millis_per_bit) % std::min<size_t>(bits, sizeof...(sequence) * 16);
    value_ = 32768 * ((sequence_[bit >> 4] >> ((~bit) & 0xf)) & 1);
  }
  int getInteger(int led) { return value_; }
  
private:
  int value_;
};

#endif
