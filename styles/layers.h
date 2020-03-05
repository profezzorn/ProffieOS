#ifndef STYLES_LAYERS_H
#define STYLES_LAYERS_H

// Usage: Layers<BASE, LAYER1, LAYER2, ...>

template<class BASE, class... LAYERS> class Layers {};

template<class BASE, class L1>
class Layers<BASE, L1> {
public:
  bool run(BladeBase* blade) {
    layer_.run(blade);
    return RunStyle(&base_, blade);
  }
private:
  BASE base_;
  L1 layer_;
public:
  auto getColor(int led) -> decltype(base_.getColor(led) << layer_.getColor(led)) {
    return base_.getColor(led) << layer_.getColor(led);
  }

};

template<class BASE, class L1, class L2, class... LAYERS>
  class Layers<BASE, L1, L2, LAYERS...> : public Layers<Layers<BASE, L1>, L2, LAYERS...> {};

#endif
