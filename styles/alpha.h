#ifndef STYLES_ALPHA_H
#define STYLES_ALPHA_H

// Usage: AlphaL<COLOR, ALPHA>
// COLOR: COLOR or LAYER
// ALPHA: FUNCTION
// Return value: LAYER
// This function makes a color transparent. The ALPHA function specifies just how opaque it should be.
// If ALPHA is zero, the returned color will be 100% transparent. If Alpha is 32768, the returned color will
// be 100% opaque. Note that if COLOR is already transparent, it will be made more transparent. Example:
// If COLOR is 50% opaque, and ALPHA returns 16384, the result will be 25% opaque.

// TODO: Make this take an arbitrary number of alpha functions and multiply them.
template<class COLOR, class ALPHA>
class AlphaL {
public:
  void run(BladeBase* blade) {
    color_.run(blade);
    alpha_.run(blade);
  }

private:
  COLOR color_;
  ALPHA alpha_;

public:
  auto getColor(int led) -> decltype(color_.getColor(led) * alpha_.getInteger(led)) {
    return color_.getColor(led) * alpha_.getInteger(led);  // clamp?
  }
};

#endif
