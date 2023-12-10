#ifndef COMMON_RESOURCES_H
#define COMMON_RESOURCES_H

namespace PO_SubSystems {
  class PO_SPI {};
  class PO_WS2811 {};
  class PO_BUTTON {};
  class PO_BLADE_DETECT {};
  class PO_DISPLAY {};
  class PO_I2S {};
  class PO_SPDIF {};
  class PO_IR {};
  class PO_PWM {};
  class PO_SERIAL {};
}

namespace PO_Resources {
  template<int p> class InputPin {};
  template<int p> class OutputPin {};
  template<int n> class SPI {};
  template<int n> class TIMER {};
}

namespace PO_ResourceTracking {

  using namespace PO_Resources;

template<int pin> struct BondedPins { static const int other = -1; };

#define PROFFIEOS_BOND_PINS(P1, P2)				   \
template<> struct BondedPins<P1> { static const int other = P2; }; \
template<> struct BondedPins<P2> { static const int other = P1; }

template <class RESOURCE, class USED_BY>
struct UseResource {
  friend USED_BY IsUsing(RESOURCE x) { return {}; }
};

template<int pin, class USER, bool INPUT = false>
struct UsePin2 {
  static_assert(sizeof(UseResource<OutputPin<pin>, USER>));
  static_assert(sizeof(UseResource<InputPin<pin>, USER>));
};

template<class USER, bool INPUT> struct UsePin2<-1, USER, INPUT> {};

template<int pin, class USER>
struct UsePin2<pin, USER, true> {
  static_assert(sizeof(UseResource<InputPin<pin>, USER>));
};


template<int pin, class USER, bool INPUT = false>
struct UseBondedPin {
  static_assert(sizeof(UseResource<OutputPin<pin>, USER>));
};

template<class USER> struct UseBondedPin<-1, USER, false> {};
template<int pin, class USER> struct UseBondedPin<pin, USER, true> {};

template<int pin, class USER, bool INPUT = false>
struct UsePin {
  static_assert(sizeof(UsePin2<pin, USER, INPUT>));
  static_assert(sizeof(UseBondedPin<BondedPins<pin>::other, USER, INPUT>));
};

}

#define USE_PIN_OUTPUT(PIN, CLASS) static_assert( sizeof( PO_ResourceTracking::UsePin<PIN, CLASS>), #CLASS)
#define USE_PIN_INPUT(PIN, CLASS) static_assert( sizeof( PO_ResourceTracking::UsePin<PIN, CLASS, true>), #CLASS)

#endif  // COMMON_RESOURCES_H
