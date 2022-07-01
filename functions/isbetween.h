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
    if (f_.getInteger(led) > bottom_.getInteger(led) && f_.getInteger(led) < top_.getInteger(led)) {
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

template<class SVFA, class SVFB, class SVFC>
class IsBetweenSVF {
 public:
  void run(BladeBase* blade) {
    svfa_.run(blade);
    svfb_.run(blade);
    svfb_.run(blade);
  }
  int calculate(BladeBase* blade) {
    if (svfa_.calculate(blade) > svfb_.calculate(blade) && svfa_.calculate(blade) < svfc_.calculate(blade)) {
      return 32768;
    } else {
      return 0;
    }
  }
 private:
  PONUA SVFA svfa_;
  PONUA SVFB svfb_;
  PONUA SVFC svfc_;
};

template<class F, class BOTTOM, class TOP> struct IsBetweenFinder { typedef IsBetweenBase<F, BOTTOM, TOP> IsBetweenClass; };
template<class F, class BOTTOM, class TOP> struct IsBetweenFinder<SingleValueAdapter<F>, SingleValueAdapter<BOTTOM>, SingleValueAdapter<TOP>> {
  typedef SingleValueAdapter<IsBetweenSVF<F, BOTTOM, TOP>> IsBetweenClass;
};
template<class F, class BOTTOM, class TOP> using IsBetween = typename IsBetweenFinder<F, BOTTOM, TOP>::IsBetweenClass;

#endif
