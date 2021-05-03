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
};

#endif
