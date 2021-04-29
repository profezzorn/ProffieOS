#ifndef COMMON_EXTRAPOLATOR_H
#define COMMON_EXTRAPOLATOR_H

template<class T>
struct ExtrapolatorData {
  T v;
  uint32_t t;
};

template<class T>
class IncrementalLine {
 private:
  uint32_t start_;
  int samples_ = 0;
  float sum_t_;
  float t_square_sum_;
  T sum_;
  T dot_sum_;
  volatile bool needs_update_ = true;
 public:
  void Start(uint32_t t) {
    start_ = t;
    samples_ = 0;
    sum_t_ = 0.0;
    t_square_sum_ = 0.0;
    sum_ = T(0.0f);
    dot_sum_ = T(0.0f);
  }
  void Add(const ExtrapolatorData<T>& data) {
    needs_update_ = true;
    samples_++;
    float t = data.t - start_;
    sum_t_ += t;
    t_square_sum_ += t * t;
    sum_ += data.v;
    dot_sum_ += data.v * t;
  }
  void Sub(const ExtrapolatorData<T>& data) {
    needs_update_ = true;
    samples_--;
    float t = data.t - start_;
    sum_t_ -= t;
    t_square_sum_ -= t * t;
    sum_ -= data.v;
    dot_sum_ -= data.v * t;
  }

 private:
  uint32_t start_copy_;
  T avg_;
  T slope_;
  float avg_t_;

  void update() {
    if (!needs_update_) return;
    needs_update_ = false;
    noInterrupts();
    start_copy_ = start_;
    float inv = 1.0f / samples_;
    avg_t_ = sum_t_ * inv;
    avg_ = sum_ * inv;
    float t_square_sum = t_square_sum_ + avg_t_ * sum_t_ - 2.0 * sum_t_ * avg_t_;
    T dot_sum = dot_sum_ - sum_ * avg_t_;
    if (t_square_sum == 0.0) {
      slope_ = T(0.0f);
    } else {
      slope_ = dot_sum * (1.0 / t_square_sum);
    }
    interrupts();
  }

 public:
  T get(uint32_t now) {
    update();
    float t = now - start_copy_;
    return avg_ + slope_ * (t - avg_t_);
  }

  T slope() {
    update();
    return slope_;
  }
  int samples() const { return samples_; }
};

template<class T, int SIZE = 10>
class Extrapolator  {
public:
  T get(uint32_t now) { return line_.get(now); }
  T get() { return get(micros()); }
  T slope() { return line_.slope(); }

  void push(const T& value, uint32_t now) {
    if (line_.samples() == 0) line_.Start(now);
    entry_++;
    if (entry_ >= SIZE) entry_ = 0;
    if (line_.samples() == SIZE) line_.Sub(data_[entry_]);
    data_[entry_].v = value;
    data_[entry_].t = now;
    line_.Add(data_[entry_]);

    if ((values_++ & 1024) == 1023) {
      // recalculate to avoid building errors
      line_.Start(data_[(entry_ + 1) % SIZE].t);
      for (size_t i = 0; i < SIZE; i++) {
	line_.Add(data_[i]);
      }
    }
  }
  void push(const T& value) {
    push(value, micros());
  }
  void clear(const T& value) {
    uint32_t now = micros();
    line_.Start(now);
    values_ = 0;
    push(value, now);
  }
  bool ready() { return line_.samples() == SIZE; }
  T& last() { return data_[entry_].v; }
  uint32_t last_time() { return data_[entry_].t; }

  void dump() {
    for (size_t i = 0; i < SIZE; i++) {
      STDOUT << " " << data_[i].t <<" " << " " << data_[i].t << "\n";
    }
  }

  ExtrapolatorData<T> data_[SIZE];
  size_t entry_ = 0;
  size_t values_ = 0;
  IncrementalLine<T> line_;
};

#endif
