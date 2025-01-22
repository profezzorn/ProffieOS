#ifndef STYLES_GET_ARG_MAX_H
#define STYLES_GET_ARG_MAX_H

#include "../functions/int.h"
#include "../functions/int_arg.h"
#include "../functions/int_select.h"
#include "../functions/mod.h"
#include "../transitions/select.h"
#include "alpha.h"
#include "color_select.h"
#include "gradient.h"
#include "../common/typelist.h"

enum SpecialArgMaxValues {
  ARG_MAX_UNKNOWN = -1,
  ARG_MAX_TIME = -2,
  ARG_MAX_ALTS = -3,
  ARG_MAX_MIXED = 4
};

constexpr int gcd(int a, int b) {
  return
    a == 0 ? 0 :
    b == 0 ? 0 :
    b > a ? gcd(b, a) :
    a % b == 0 ? b :
    gcd(b, a % b);
}

constexpr int lcm(int a, int b) {
  return
    ((((uint64_t)a) * ((uint64_t)b)) / gcd(a, b));
}

constexpr int combine_args(int a, int b) {
  return
    a == ARG_MAX_UNKNOWN ? b :
    b == ARG_MAX_UNKNOWN ? a :
    a == b ? a :
    a < 0 || b < 0 ? ARG_MAX_MIXED :
    lcm(a, b);
}


template<class T, int N> struct GetArgMax { static const int value = -1; };

// Get max arg for all entries in a TypeList
template <int ARG, class TL>
struct GetArgMaxTL {
  static const int fh = GetArgMaxTL<ARG, typename SplitTypeList<TL>::first_half>::value;
  static const int sh = GetArgMaxTL<ARG, typename SplitTypeList<TL>::second_half>::value;
  static const int value = combine_args(fh, sh);
};

template<int ARG>
struct GetArgMaxTL<ARG, TypeList<>> { static const int value = -1; };

// Get max arg for a template invocation
template <int ARG, template<class...> class F, class ... REST> struct GetArgMaxT {
  static const int value = GetArgMaxTL<ARG, TypeList<REST...>>::value;
};

template<int ARG, int DEFAULT_VALUE, class ... REST>
struct GetArgMaxT<ARG, ColorSelect, IntArg<ARG, DEFAULT_VALUE>, REST...> {
  static const int A = sizeof...(REST) - 1;  // don't count the transition
  static const int B = GetArgMax<Gradient<REST...>, ARG>::value;
  static const int value = combine_args(A, B);
};

template<int ARG, int DEFAULT_VALUE, class ... REST>
struct GetArgMaxT<ARG, TrSelect, IntArg<ARG, DEFAULT_VALUE>, REST...> {
  static const int A = sizeof...(REST);
  static const int B = GetArgMax<TrRandom<REST...>, ARG>::value;
  static const int value = combine_args(A, B);
};

template<int ARG, int DEFAULT_VALUE, int MAX>
struct GetArgMax< SingleValueAdapter<ModSVF<IntArgSVF<ARG, DEFAULT_VALUE>, IntSVF<MAX> >>, ARG> { static const int value = MAX; };

template<class X, int ARG, int DEFAULT_VALUE>
struct GetArgMaxT<ARG, AlphaL, X, IntArg<ARG, DEFAULT_VALUE>> {
  static const int value = 32769;
};

template<int ARG, template<class...> class F, class ... REST>
struct GetArgMax<F<REST...>, ARG> {
  static const int value =  GetArgMaxT<ARG, F, REST...>::value;
};

// Matches TransitionEffectL
template<int ARG, template<class, BladeEffectType> class F, class A, BladeEffectType B>
struct GetArgMax<F<A, B>, ARG> {
  static const int value =  GetArgMax<A, ARG>::value;
};

template<int ARG, template<class, class, int> class F, class A, class B, int C>
struct GetArgMax<F<A, B, C>, ARG> {
  static const int value =  GetArgMaxTL<ARG, TypeList<A, B>>::value;
};

template<int ARG, template<class, class, class, bool> class F, class A, class B, class C, bool D>
struct GetArgMax<F<A, B, C, D>, ARG> {
  static const int value =  GetArgMaxTL<ARG, TypeList<A, B, C>>::value;
};

template<int ARG, int DEFAULT_VALUE, int ... REST>
struct GetArgMax<IntSelect<IntArg<ARG, DEFAULT_VALUE>, REST...>, ARG> {
  static const int value = sizeof...(REST);
};

template<int ARG, class T>
struct GetArgMaxTL<ARG, TypeList<T>> {
  static const int value = GetArgMax<T, ARG>::value;
};


#endif
