#ifndef COMMON_CYCLINT_H
#define COMMON_CYCLINT_H

// Lets you do comparisons on integers that wrap around.
// T should be an unsigned type.
template<typename T>
struct Cyclint {
  Cyclint() : v(0) {}
  static const T NMASK = ((T)1) << (sizeof(T)*8 - 1);
  explicit Cyclint(T value) : v(value) {}
  Cyclint operator+(T o) { return Cyclint(v+o); }
  Cyclint operator+(int o) { return Cyclint(v+o); }
  Cyclint operator-(T o) { return Cyclint(v-o); }
  Cyclint operator-(int o) { return Cyclint(v-o); }
  void operator+=(T o) { v += o; }
  void operator-=(T o) { v -= o; }
  bool operator==(Cyclint o) const { return v == o.v; }
  bool operator!=(Cyclint o) const { return v != o.v; }
  bool operator<(Cyclint o) const { return !!((v - o.v) & NMASK); }
  bool operator<=(Cyclint o) const { return !((o.v - v) & NMASK); }
  bool operator>(Cyclint o) const { return !!((o.v - v) & NMASK); }
  bool operator>=(Cyclint o) const { return !((v - o.v) & NMASK); }
  operator const T() const { return v; }
  T v;
};

#endif  // COMMON_CYCLINT_H
