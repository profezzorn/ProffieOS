#ifndef STYLES_ALPHA_H
#define STYLES_ALPHA_H

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
