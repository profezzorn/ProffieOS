#ifndef COMMON_RANGE_H
#define COMMON_RANGE_H

class Range {
public:
  uint32_t start;
  uint32_t end;
  Range() : start(0), end(0) {}
  Range(uint32_t s, uint32_t e) : start(s), end(e) {}
  uint32_t size() const {
    if (start >= end) return 0;
    return end - start;
  }
  Range operator&(const Range& other) {
    return Range(std::max(start, other.start),
                 std::min(end, other.end));
  }

  // Stripes repeat at some interval infinitely many times.
  // These functions lets you calculate the intersection between
  // stripes and some range.

  // Stripes are Range(N*mod, N*mod + stripe_width)
  // Returns filled area from 0...end
  static uint32_t intersect_with_stripes(uint32_t stripe_width, uint32_t mod, uint32_t end) {
    return (end / mod) * stripe_width + std::min<uint32_t>(stripe_width, end % mod);  
  }
  
  // Stripes are Range(N*mod, N*mod + stripe_width)
  // Returns filled area from start...end
  uint32_t intersect_with_stripes(uint32_t stripe_width, uint32_t mod) const {
    return intersect_with_stripes(stripe_width, mod, end) - intersect_with_stripes(stripe_width, mod, start);
  }

  // Stripes are Range(N*mod + stripe.start, N*mod + stripe.end) (N can be negative)
  // Returns filled area from start ... end
  uint32_t intersect_with_stripes(Range stripe, uint32_t mod) const {
    // Convert into a stripe which starts at 0 and then call function above.
    uint32_t shift = mod - stripe.start;
    Range tmp(start + shift, end + shift);
    return tmp.intersect_with_stripes(stripe.size(), mod);
  }
};

#endif
