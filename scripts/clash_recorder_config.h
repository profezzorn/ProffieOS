#ifdef CONFIG_PROP
#ifndef PROP_TYPE
#include "../props/saber.h"
#endif
#include "clash_recorder.h"
using PROP_TYPE_TMP = PROP_TYPE;
#undef PROP_TYPE
#define PROP_TYPE ClashRecorder<PROP_TYPE_TMP>
#endif

#ifdef CONFIG_PRESETS

class ShowLocationF {
public:
  void run(BladeBase *blade) {}
  int getInteger(int led) { return pos_; }
  static void SetPos(int x) { pos_ = x; }
private:
  static int pos_;
};

int ShowLocationF::pos_ = 0;

using ClashRecorderStyle = InOutHelper<
  EasyBlade< Layers<Red, AlphaL<White, Bump<ShowLocationF, Int<4096>>>>, White>,
  800, 300>;

#endif
