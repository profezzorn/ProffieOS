#ifndef COMMON_CAPABILITIES_H
#define COMMON_CAPABILITIES_H

#include "common.h"

#define CREATE_ENUM_CHECKER(ID)						\
template <class ENUM>							\
class EnumContains_##ID {						\
private:								\
    template <class E, typename Enable = void>				\
    struct test2 {							\
      static const bool exists = false;                                 \
      static const int value = -9999;					\
    };									\
    template <class E>							\
      struct test2<E, typename ToVoid<decltype(E::ID)*>::Type > {	\
      static const bool exists = true;                                  \
      static const int value = E::ID;					\
    };									\
									\
public:									\
    static constexpr bool exists = test2<ENUM>::exists;			\
    static constexpr int value = test2<ENUM>::value;			\
};

// Expand to numbers higher than 10 if required.
CREATE_ENUM_CHECKER(bladePowerPin1);
CREATE_ENUM_CHECKER(bladePowerPin2);
CREATE_ENUM_CHECKER(bladePowerPin3);
CREATE_ENUM_CHECKER(bladePowerPin4);
CREATE_ENUM_CHECKER(bladePowerPin5);
CREATE_ENUM_CHECKER(bladePowerPin6);
CREATE_ENUM_CHECKER(bladePowerPin7);
CREATE_ENUM_CHECKER(bladePowerPin8);
CREATE_ENUM_CHECKER(bladePowerPin9);
CREATE_ENUM_CHECKER(bladePowerPin10);

constexpr bool IsPWMPin(int pin) {
  return
    pin == EnumContains_bladePowerPin1<SaberPins>::value ||
    pin == EnumContains_bladePowerPin2<SaberPins>::value ||
    pin == EnumContains_bladePowerPin3<SaberPins>::value ||
    pin == EnumContains_bladePowerPin4<SaberPins>::value ||
    pin == EnumContains_bladePowerPin5<SaberPins>::value ||
    pin == EnumContains_bladePowerPin6<SaberPins>::value ||
    pin == EnumContains_bladePowerPin7<SaberPins>::value ||
    pin == EnumContains_bladePowerPin8<SaberPins>::value ||
    pin == EnumContains_bladePowerPin9<SaberPins>::value ||
    pin == EnumContains_bladePowerPin10<SaberPins>::value;
}

#endif
