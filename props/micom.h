#ifndef PROPS_AUDIOFX_H
#define PROPS_AUDIOFX_H

#include "prop_base.h"

RefPtr<BufferedWavPlayer> current_player_;
Effect* now_playing_;
Effect* play_next_;
class TriggerEffectGroup;
TriggerEffectGroup* current_question_;

#define COMMA ,

#define SUBNAMES(X, SEPARATOR)			\
  X(click)					\
  SEPARATOR                                     \
  X(hold)					\
  SEPARATOR                                     \
  X(question)					\
  SEPARATOR                                     \
  X(answer1)					\
  SEPARATOR                                     \
  X(answer2)					\
  SEPARATOR                                     \
  X(answer3)					\
  SEPARATOR                                     \
  X(answer4)

#define CONST_CHAR_NAME(X) const char* X##_name
#define INIT_EFFECT(X) X##_(X##_name)
#define DECLARE_EFFECT(X) Effect X##_;

void Play(Effect* f) {
  if (current_player_ && current_player_->isPlaying()) {
    if (now_playing_ == f) return;
    play_next_ = f;
    return;
  }
  if (!current_player_) current_player_ = GetFreeWavPlayer();
  if (!current_player_) return;
  f->SelectNext();  // Play in order
  current_player_->PlayOnce(f);
  now_playing_ = f;
}

class TriggerEffectGroup {
public:
  TriggerEffectGroup(SUBNAMES(CONST_CHAR_NAME, COMMA)) : SUBNAMES(INIT_EFFECT, COMMA) {}

  SUBNAMES(DECLARE_EFFECT, );

  
  bool Event(EVENT event) {
    if (event == EVENT_FIRST_SAVED_CLICK_SHORT) {
      if (click_) Play(&click_);
      return true;
    }
    if (event == EVENT_HELD_LONG) {
      if (question_) {
	Play(&question_);
	current_question_ = this;
	return true;
      }
      if (hold_) {
	Play(&hold_);
	return true;
      }
    }
    return false;
  }

  bool Answer(enum BUTTON button) {
    switch (button) {
      case BUTTON_TRIGGER_ONE:
	if (answer1_) {
	  // Repeat question, so we're still in the same question mode.
	  Play(&answer1_);
	  return true;
	}
	break;
      case BUTTON_TRIGGER_TWO:
	if (answer2_) {
	  Play(&answer2_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_THREE:
	if (answer3_) {
	  Play(&answer3_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_FOUR:
	if (answer4_) {
	  Play(&answer4_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_EIGHT:
	// No answer, just cancel.
	  current_question_ = nullptr;
	return true;
	
      default: break;
    }
    return false;
  }
};

#define DEFINE_TRIGGER(X)			\
TriggerEffectGroup trigger##X(                  \
		   "t" #X "clk",		\
		   "t" #X "hld",		\
		   "t" #X "q",		        \
                   "t" #X "a1",			\
                   "t" #X "a2",			\
                   "t" #X "a3",			\
                   "t" #X "a4")

DEFINE_TRIGGER(1);
DEFINE_TRIGGER(2);
DEFINE_TRIGGER(3);
DEFINE_TRIGGER(4);
DEFINE_TRIGGER(5);
DEFINE_TRIGGER(6);
DEFINE_TRIGGER(7);
DEFINE_TRIGGER(8);

#define PROP_TYPE AudioFx

class AudioFx : public PropBase {
public:
  const char* name() override { return "MiCOM"; }
#if NUM_BUTTONS >= 2
  // Make clash do nothing
  void Clash(bool stab) override {}
#endif
  void Loop() {
    PropBase::Loop();
    bool playing = current_player_ && current_player_->isPlaying();
    if (current_player_ && !playing && play_next_) {
      Effect* tmp = play_next_;
      play_next_ = nullptr;
      Play(tmp);
      play_next_ = NULL;
      playing = true;
    }
    hybrid_font.SetHumVolume(1.0);
  }

  // Make swings do nothing
  void DoMotion(const Vec3& motion, bool clear) override { }

  bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) override {
    // If there is a hum, we need to turn on.
    // Make any event turn it on.
    if (event == EVENT_FIRST_SAVED_CLICK_SHORT) {
      if ((SFX_hum || SFX_humm) && !SaberBase::IsOn()) {
	On();
	return true;
      }
      
      if (current_question_) {
	if (current_question_->Answer(button)) {
	  return true;
	}
      }
    }

    switch (EVENTID(button, event, modifiers & ~MODE_ON)) {
      case EVENTID(BUTTON_TRIGGER_ONE, EVENT_HELD_LONG, BUTTON_TRIGGER_EIGHT):
      case EVENTID(BUTTON_TRIGGER_EIGHT, EVENT_HELD_LONG, BUTTON_TRIGGER_ONE):
	if (SaberBase::IsOn()) {
	  Off();
	  return true;
	}
	next_preset();
	return true;

      case EVENTID(BUTTON_TRIGGER_EIGHT, EVENT_HELD_LONG, 0):
        StartOrStopTrack();
	return true;
    }
    
    switch (button) {
      case BUTTON_TRIGGER_ONE:   return trigger1.Event(event);
      case BUTTON_TRIGGER_TWO:   return trigger2.Event(event);
      case BUTTON_TRIGGER_THREE: return trigger3.Event(event);
      case BUTTON_TRIGGER_FOUR:  return trigger4.Event(event);
      case BUTTON_TRIGGER_FIVE:  return trigger5.Event(event);
      case BUTTON_TRIGGER_SIX:   return trigger6.Event(event);
      case BUTTON_TRIGGER_SEVEN: return trigger7.Event(event);
      case BUTTON_TRIGGER_EIGHT: return trigger8.Event(event);
      default: break;
    }
    return false;
  }
};


#endif
