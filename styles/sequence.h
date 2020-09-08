#ifndef STYLES_SEQUENCE_H
#define STYLES_SEQUENCE_H

// usage: Sequence<COLOR1, COLOR2, int millis_per_bits, int bits, 0b0000000000000000, ....>
// COLOR1: COLOR
// COLOR2: COLOR
// millis_per_bit: millseconds spent on each bit
// bits: number of bits before we loop around to the beginning
// 0b0000000000000000: 16-bit binary numbers containing the actual sequence.
//
// Shows COLOR1 if the current bit in the sequence is 1, COLOR2 otherwise.
// The number of 16-bit binary numbers should be at least |bits| / 16, rounded up.
// Note that if not all bits are used within the 16-bit number.
// Example, a red SOS pattern:
// Sequence<RED, BLACK, 100, 37, 0b0001010100011100, 0b0111000111000101, 0b0100000000000000>

#include "../functions/sequence.h"

template<class COLOR2, int millis_per_bit, int bits, int ... sequence >
  using SequenceL = AlphaL<COLOR2, SequenceF<millis_per_bit, bits, sequence...>>;

template<class COLOR1, class COLOR2, int millis_per_bit, int bits, int ... sequence >
  using Sequence = Layers<COLOR2, SequenceL<COLOR1, millis_per_bit, bits, sequence...>>;


template<int millis_per_color, class... COLORS> 
class ColorSequence {
public:
  void run(BladeBase* blade) {
    colors_.run(blade);
    uint32_t now = micros();
    if (now - last_micros_ > millis_per_color * 1000) {
      if (now - last_micros_ > millis_per_color * 10000) {
	n_ = 0;
	last_micros_ = now;
      } else {
	n_ = (n_ + 1) % sizeof...(COLORS);
	last_micros_ += millis_per_color * 1000;
      }
    }
  }
private:
  uint32_t last_micros_;
  int n_;
  MixHelper<COLORS...> colors_;

public:  
  auto getColor(int led) -> decltype(colors_.getColor(n_, led)) {
    return colors_.getColor(n_, led);
  }
};

#endif
