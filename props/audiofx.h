#ifndef PROPS_AUDIOFX_H
#define PROPS_AUDIOFX_H

#include "prop_base.h"

RefPtr<BufferedWavPlayer> current_player_;

class TriggerEffectGroup {
public:
  TriggerEffectGroup(const char* base_name,
		     const char* hold_name,
		     const char* latch_name,
		     const char* next_name,
		     const char* rand_name) :
  base_(base_name),
    hold_(hold_name),
    latch_(latch_name),
    next_(next_name),
    rand_(rand_name) {
  }

  void Stop() {
    if (!current_player_) return;
    current_player_->set_fade_time(0.02f);
    current_player_->FadeAndStop();
    current_player_.Free();
  }
  void Play(Effect* f) {
    Stop();
    current_player_ = GetFreeWavPlayer();
    if (!current_player_) return;
    current_player_->PlayOnce(f);
  }
  void PlayLoop(Effect* f) {
    Play(f);
    if (!current_player_) return;
    current_player_->PlayLoop(f);
  }

  Effect base_;
  Effect hold_;
  Effect latch_;
  Effect next_;
  Effect rand_;

  bool playing_ = false;

  bool Event(EVENT event) {
    if (event == EVENT_PRESSED) {
      if (base_) {
	Play(&base_);
      } else if (rand_) {
	Play(&rand_);
      } else if (next_) {
	next_.SelectNext();
	Play(&next_);
      } else if (latch_) {
	if (playing_) {
	  PlayLoop(&latch_);
          playing_ = true;
	} else {
          playing_ = false;
	  Stop();
	}
      } else if (hold_) {
	PlayLoop(&hold_);
      }
      return true;
    }
    if (event == EVENT_RELEASED) {
      if (hold_) {
	Stop();
	return true;
      }
    }
    return false;
  }
};

#define DEFINE_TRIGGER(X)			\
TriggerEffectGroup trigger##X("t" #X,		\
		   "t" #X "hold",		\
		   "t" #X "latch",		\
		   "t" #X "next",		\
                   "t" #X "rand")

DEFINE_TRIGGER(00);
DEFINE_TRIGGER(01);
DEFINE_TRIGGER(02);
DEFINE_TRIGGER(03);
DEFINE_TRIGGER(04);
DEFINE_TRIGGER(05);
DEFINE_TRIGGER(06);
DEFINE_TRIGGER(07);
DEFINE_TRIGGER(08);
DEFINE_TRIGGER(09);
DEFINE_TRIGGER(10);

#define PROP_TYPE AudioFx

class AudioFx : public PROP_INHERIT_PREFIX PropBase {
public:
  const char* name() override { return "AudioFx"; }
#if NUM_BUTTONS >= 2
  // Make clash do nothing
  void Clash(bool stab, float strength) override {}
#endif

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override { }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    switch (button) {
      case BUTTON_TRIGGER_ONE:   return trigger00.Event(event);
      case BUTTON_TRIGGER_TWO:   return trigger01.Event(event);
      case BUTTON_TRIGGER_THREE: return trigger02.Event(event);
      case BUTTON_TRIGGER_FOUR:  return trigger03.Event(event);
      case BUTTON_TRIGGER_FIVE:  return trigger04.Event(event);
      case BUTTON_TRIGGER_SIX:   return trigger05.Event(event);
      case BUTTON_TRIGGER_SEVEN: return trigger06.Event(event);
      case BUTTON_TRIGGER_EIGHT: return trigger07.Event(event);
      default: break;
    }
    return false;
  }
};


#endif
