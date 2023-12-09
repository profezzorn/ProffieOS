#ifndef COMMON_CIRCULAR_BUFFER_H
#define COMMON_CIRCULAR_BUFFER_H

#include "atomic.h"

template<class T, size_t SIZE>
class CircularBuffer {
public:
  CircularBuffer() : first_(0), last_(0) {}

  T& current() { return data_[last_.get() % SIZE]; }
  T* data() { return data_ + last_.get() % SIZE; }
  T* space() { return data_ + first_.get() % SIZE; }
  T& next() { return data_[first_.get() % SIZE]; }

  size_t size() const { return first_.get() - last_.get(); }
  bool empty() const { return size() == 0; }
  size_t space_available() const { return SIZE - size(); }
  size_t continuous_space() const {
    return std::min<size_t>(space_available(), SIZE - first_.get() % SIZE);
  }
  size_t continuous_data() const {
    return std::min<size_t>(size(), SIZE - last_.get() % SIZE);
  }

  void push() { first_ += 1; }
  void push(size_t elements) { first_ += elements; }
  T pop() {
    T ret = current();
    last_ += 1;
    return ret;
  }
  void pop(size_t elements) { last_ += elements; }

  void clear() { first_ = 0; last_=0; }
  size_t pos() { return last_.get(); }

private:
  T data_[SIZE];
  POAtomic<size_t> first_;
  POAtomic<size_t> last_;
};

#endif  // COMMON_CIRCULAR_BUFFER_H
