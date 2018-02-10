#ifndef STYLES_STROBE_H
#define STYLES_STROBE_H

template<class T, class STROBE_COLOR, int STROBE_FREQUENCY, int STROBE_MILLIS = 1>
class Strobe {
public:
   void run(BladeBase* blade) {
     base_.run(blade);
     strobe_color_.run(blade);
     uint32_t m = millis();
     uint32_t timeout = strobe_ ? STROBE_MILLIS : (1000/STROBE_FREQUENCY);
     if (m - strobe_millis_ > timeout) {
        strobe_millis_ += timeout;
        if (m - strobe_millis_ > STROBE_MILLIS + (1000/STROBE_FREQUENCY))
          strobe_millis_ = m;
        strobe_ = !strobe_;
     }
   }
   OverDriveColor getColor(int led) {
     if (strobe_) {
        return strobe_color_.getColor(led);
     } else {
        return base_.getColor(led);
     }
   }
private:
  bool strobe_;
  T base_;
  STROBE_COLOR strobe_color_;
  uint32_t strobe_millis_;
};

#endif
