#ifndef SOUND_FILTER_H
#define SOUND_FILTER_H
#include <complex>

namespace Filter {

#ifndef FILTER_ORDER
#define FILTER_ORDER 8
#endif

struct C {
  constexpr C(double real = 0.0, double imag = 0.0) : real_(real), imag_(imag) {}
  constexpr C operator+(C other) { return C(real_ + other.real_, imag_ + other.imag_); }
  constexpr C operator*(C other) {
    return C((real_ * other.real_ - imag_ * other.imag_),
	     (real_ * other.imag_ + imag_ * other.real_));
  }
  constexpr C operator-(C other) { return C(real_ - other.real_, imag_ - other.imag_); }
  constexpr C operator/(C other) {
    return C( (real_ * other.real_ + imag_ * other.imag_) / other.len2(),
	      (imag_ * other.real_ - real_ * other.imag_) / other.len2());
  }
  constexpr double real() const { return real_; }
  constexpr double imag() const { return imag_; }
  constexpr double abs() const { return sqrt(len2()); }
private:
  constexpr double len2() const { return real_*real_ + imag_*imag_; }
  double real_;
  double imag_;
};

template<int ORDER>
struct ButterWorthProtoType {
  static const size_t order = ORDER;
  static const size_t poles = 2 * ((order + 1)/2);
  static const size_t zeroes = 0;

  static constexpr double theta(size_t k) {
    return (2 * k + 1) * M_PI / (2 * order);
  }
  
  static constexpr double real(size_t k) { return -sin(theta(k)); }
  static constexpr double imag(size_t k) { return cos(theta(k)); }
  
  static constexpr C pole(size_t k) {
    return (k & 1) ?
      C(real(k/2), imag(k/2)) :
      C(real(k/2), -imag(k/2));
  }
  static constexpr C zero(size_t k) {
    return C(0.0, 0.0);
  }
  static constexpr double gain() { return 1.0; }
};


template<class T, int cutoff_frequency, int sampling_frequency>
struct ConvertToHighPass {
  static const size_t poles = T::poles;
  static const size_t zeroes = T::poles;
  static constexpr double f() { return 2 * tan(M_PI * cutoff_frequency / sampling_frequency); }
  static constexpr C pole(size_t k) { return C(f()) / T::pole(k); }
  static constexpr C zero(size_t k) { return C(0.0, 0.0); }
  static constexpr double gain() { return T::gain(); }
};


template<class T>
struct BLT {
  static const size_t poles = T::poles;
  static const size_t zeroes = T::zeroes;
  static constexpr C blt(C s) { return (C(2) + s) / (C(2) - s); }
  static constexpr C pole(size_t k) { return blt(T::pole(k)); }
  static constexpr C zero(size_t k) { return blt(T::zero(k)); }

  static constexpr double gain_from_pole(size_t k) {
    return (C(2) - T::pole(k)).abs();
  }
  static constexpr double gain_from_zero(size_t k) {
    return (C(2) - T::zero(k)).abs();
  }
  static constexpr double pole_gain_prod(size_t i = 0) {
    return i == T::poles ? 1.0 : gain_from_pole(i) * pole_gain_prod(i + 1);
  }
  static constexpr double zero_gain_prod(size_t i = 0) {
    return i == T::zeroes ? 1.0 : gain_from_zero(i) * zero_gain_prod(i + 1);
  }
  static constexpr double gain() {
    return T::gain() * pole_gain_prod() / zero_gain_prod();
  }
};

template<class T>
struct Bilinear {
  // currently, order must be even!
  static const size_t order = T::poles > T::zeroes ? T::poles : T::zeroes;
  static const size_t biquads = (order + 1) / 2;
  static constexpr double gain = T::gain();
  static constexpr double b0(size_t k) { return 1.0; }
  static constexpr double b1(size_t k) { return -(T::zero(k * 2) + T::zero(k * 2 + 1)).real(); }
  static constexpr double b2(size_t k) { return  (T::zero(k * 2) * T::zero(k * 2 + 1)).real(); }
  static constexpr double a0(size_t k) { return 1.0; }
  static constexpr double a1(size_t k) { return -(T::pole(k * 2) + T::pole(k * 2 + 1)).real(); }
  static constexpr double a2(size_t k) { return  (T::pole(k * 2) * T::pole(k * 2 + 1)).real(); }
};


#if 1
#define FILTER_TYPE float

  template<class T, size_t k>
struct BQ {
  static constexpr FILTER_TYPE b0 = T::b0(k);
  static constexpr FILTER_TYPE b1 = T::b1(k);
  static constexpr FILTER_TYPE b2 = T::b2(k);
  static constexpr FILTER_TYPE a0 = T::a0(k);
  static constexpr FILTER_TYPE a1 = T::a1(k);
  static constexpr FILTER_TYPE a2 = T::a2(k);
};

#define BIQUAD(X, XN, XN1, XN2, YN1, YN2) \
  BQ<T,X>::b0 * XN + BQ<T,X>::b1 * XN1 + BQ<T,X>::b2 * XN2 - BQ<T,X>::a1 * YN1 - BQ<T,X>::a2 * YN2

#define FILTER_FROM_FLOAT(X) X
#define FILTER_TO_FLOAT(X) X

#endif

#if 0
template<class T, size_t k>
struct BQ {
  static constexpr uint32_t SHIFT = 30;
  static constexpr uint32_t MULT = 1 << SHIFT;
  static constexpr int32_t b0 = T::b0(k) * MULT;
  static constexpr int32_t b1 = T::b1(k) * MULT;
  static constexpr int32_t b2 = T::b2(k) * MULT;
  static constexpr int32_t a0 = T::a0(k) * MULT;
  static constexpr int32_t a1 = T::a1(k) * MULT;
  static constexpr int32_t a2 = T::a2(k) * MULT;
};

#define BIQUAD(X, XN, XN1, XN2, YN1, YN2) \
  ((int64_t)XN * BQ<T,X>::b0 + (int64_t)XN1 * BQ<T,X>::b1 + (int64_t)XN2 * BQ<T,X>::b2 - (int64_t)YN1 * BQ<T,X>::a1 - (int64_t)YN2 * BQ<T,X>::a2) >> BQ<T,X>::SHIFT;

#define FILTER_TYPE int32_t
#define FILTER_FROM_FLOAT(X) ((X) * 16.0)
#define FILTER_TO_FLOAT(X) ((X) * (1.0/16.0))
  
#endif

#if 0
template<class T, size_t k>
struct BQ {
  static constexpr uint32_t SHIFT = 30;
  static constexpr uint32_t MULT = 1 << SHIFT;
  static constexpr int32_t b0 = T::b0(k) * MULT;
  static constexpr int32_t b1 = T::b1(k) * MULT;
  static constexpr int32_t b2 = T::b2(k) * MULT;
  static constexpr int32_t a0 = T::a0(k) * MULT;
  static constexpr int32_t a1 = T::a1(k) * MULT;
  static constexpr int32_t a2 = T::a2(k) * MULT;
};

#define FILTER_MULT(X, Y) (int32_t)((((int64_t) X) * Y) >> 32)

#define BIQUAD(X, XN, XN1, XN2, YN1, YN2) \
  (FILTER_MULT(XN, (BQ<T,X>::b0)) + FILTER_MULT(XN1, (BQ<T,X>::b1)) + FILTER_MULT(XN2, (BQ<T,X>::b2)) - FILTER_MULT(YN1,(BQ<T,X>::a1)) - FILTER_MULT(YN2, (BQ<T,X>::a2))) << 2

#define FILTER_TYPE int32_t
#define FILTER_FROM_FLOAT(X) ((X) * 16.0)
#define FILTER_TO_FLOAT(X) ((X) * (1.0/16.0))
#endif


#define BIQUAD_BLOCK(X, Y)						\
  FILTER_TYPE D##Y##x0 = data_[Y][0];						\
  FILTER_TYPE D##Y##x1 = data_[Y][1];						\
  FILTER_TYPE D##Y##x2 = BIQUAD(X, D##X##x2, D##X##x1, D##X##x0, D##Y##x1, D##Y##x0);	\
  FILTER_TYPE D##Y##x3 = BIQUAD(X, D##X##x3, D##X##x2, D##X##x1, D##Y##x2, D##Y##x1);	\
  FILTER_TYPE D##Y##x4 = BIQUAD(X, D##X##x4, D##X##x3, D##X##x2, D##Y##x3, D##Y##x2);	\
  FILTER_TYPE D##Y##x5 = BIQUAD(X, D##X##x5, D##X##x4, D##X##x3, D##Y##x4, D##Y##x3);	\
  data_[Y][0] = D##Y##x4;						\
  data_[Y][1] = D##Y##x5

template<class T>
class Biquad {
 public:
  FILTER_TYPE data_[T::biquads + 1][2];
  Biquad() { clear(); }
  void clear() {
    for (size_t i = 0; i <= T::biquads; i++) {
      data_[i][0] = 0;
      data_[i][1] = 0;
    }
  }
  void Run4(float* inout) {
    FILTER_TYPE D0x0 = data_[0][0];
    FILTER_TYPE D0x1 = data_[0][1];
    FILTER_TYPE D0x2 = FILTER_FROM_FLOAT(inout[0]);
    FILTER_TYPE D0x3 = FILTER_FROM_FLOAT(inout[1]);
    FILTER_TYPE D0x4 = FILTER_FROM_FLOAT(inout[2]);
    FILTER_TYPE D0x5 = FILTER_FROM_FLOAT(inout[3]);
    data_[0][0] = D0x4;
    data_[0][1] = D0x5;
    
    BIQUAD_BLOCK(0, 1);
#if FILTER_ORDER > 2
    BIQUAD_BLOCK(1, 2);
#endif
#if FILTER_ORDER > 4
    BIQUAD_BLOCK(2, 3);
#endif
#if FILTER_ORDER > 6
    BIQUAD_BLOCK(3, 4);
#endif
#if FILTER_ORDER > 8
    BIQUAD_BLOCK(4, 5);
#endif
#if FILTER_ORDER > 10
    BIQUAD_BLOCK(5, 6);
#endif
#if FILTER_ORDER > 12
    BIQUAD_BLOCK(6, 7);
#endif
#if FILTER_ORDER > 14
    BIQUAD_BLOCK(7, 8);
#endif

#if FILTER_ORDER <= 2
    inout[0] = FILTER_TO_FLOAT(D1x2);
    inout[1] = FILTER_TO_FLOAT(D1x3);
    inout[2] = FILTER_TO_FLOAT(D1x4);
    inout[3] = FILTER_TO_FLOAT(D1x5);
#elif FILTER_ORDER <= 4
    inout[0] = FILTER_TO_FLOAT(D2x2);
    inout[1] = FILTER_TO_FLOAT(D2x3);
    inout[2] = FILTER_TO_FLOAT(D2x4);
    inout[3] = FILTER_TO_FLOAT(D2x5);
#elif FILTER_ORDER <= 6
    inout[0] = FILTER_TO_FLOAT(D3x2);
    inout[1] = FILTER_TO_FLOAT(D3x3);
    inout[2] = FILTER_TO_FLOAT(D3x4);
    inout[3] = FILTER_TO_FLOAT(D3x5);
#elif FILTER_ORDER <= 8
    inout[0] = FILTER_TO_FLOAT(D4x2);
    inout[1] = FILTER_TO_FLOAT(D4x3);
    inout[2] = FILTER_TO_FLOAT(D4x4);
    inout[3] = FILTER_TO_FLOAT(D4x5);
#elif FILTER_ORDER <= 10
    inout[0] = FILTER_TO_FLOAT(D5x2);
    inout[1] = FILTER_TO_FLOAT(D5x3);
    inout[2] = FILTER_TO_FLOAT(D5x4);
    inout[3] = FILTER_TO_FLOAT(D5x5);
#elif FILTER_ORDER <= 12
    inout[0] = FILTER_TO_FLOAT(D6x2);
    inout[1] = FILTER_TO_FLOAT(D6x3);
    inout[2] = FILTER_TO_FLOAT(D6x4);
    inout[3] = FILTER_TO_FLOAT(D6x5);
#elif FILTER_ORDER <= 14
    inout[0] = FILTER_TO_FLOAT(D7x2);
    inout[1] = FILTER_TO_FLOAT(D7x3);
    inout[2] = FILTER_TO_FLOAT(D7x4);
    inout[3] = FILTER_TO_FLOAT(D7x5);
#elif FILTER_ORDER <= 16
    inout[0] = FILTER_TO_FLOAT(D8x2);
    inout[1] = FILTER_TO_FLOAT(D8x3);
    inout[2] = FILTER_TO_FLOAT(D8x4);
    inout[3] = FILTER_TO_FLOAT(D8x5);
#else
#error UNSUPPORTED FILTER ORDER
#endif
    FILTER_TYPE gain_inv = 1.0 / T::gain;
    inout[0] *= gain_inv;
    inout[1] *= gain_inv;
    inout[2] *= gain_inv;
    inout[3] *= gain_inv;
  }
};

}  // namespace Filter

#endif
