#ifndef FUNCTIONS_ISBETWEEN_H
#define FUNCTIONS_ISBETWEEN_H

// Usage: IsBetween<F, BOTTOM, TOP>
// Returns 0 or 32768 based F > BOTTOM and < TOP 
// F, BOTTOM, TOP: INTEGER
// return value: INTEGER

class BladeBase;

template<class F, class BOTTOM, class TOP>
class IsBetweenBase {
public:
  void run(BladeBase* blade) {
    f_.run(blade);
    bottom_.run(blade);
    top_.run(blade);
  }
  int getInteger(int led) {
    int f = f_.getInteger(led);
    if (f > bottom_.getInteger(led) && f < top_.getInteger(led)) {
      return 32768;
    } else {
      return 0;
    }
  }

private:
  PONUA F f_;
  PONUA BOTTOM bottom_;
  PONUA TOP top_;
};

template<class SVF_F, class SVF_BOTTOM, class SVF_TOP>
class IsBetweenSVF {
 public:
  void run(BladeBase* blade) {
    svf_f_.run(blade);
    svf_bottom_.run(blade);
    svf_top_.run(blade);
  }
  int calculate(BladeBase* blade) {
    int f = svf_f_.calculate(blade);
    if (f > svf_bottom_.calculate(blade) && f < svf_top_.calculate(blade)) {
      return 32768;
    } else {
      return 0;
    }
  }
 private:
  PONUA SVF_F svf_f_;
  PONUA SVF_BOTTOM svf_bottom_;
  PONUA SVF_TOP svf_top_;
};

template<class F, class BOTTOM, class TOP> struct IsBetweenFinder { typedef IsBetweenBase<F, BOTTOM, TOP> IsBetweenClass; };
template<class F, class BOTTOM, class TOP> struct IsBetweenFinder<SingleValueAdapter<F>, SingleValueAdapter<BOTTOM>, SingleValueAdapter<TOP>> {
  typedef SingleValueAdapter<IsBetweenSVF<F, BOTTOM, TOP>> IsBetweenClass;
};
template<class F, class BOTTOM, class TOP> using IsBetween = typename IsBetweenFinder<F, BOTTOM, TOP>::IsBetweenClass;

#endif
