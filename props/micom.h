#ifndef PROPS_AUDIOFX_H
#define PROPS_AUDIOFX_H

#include "prop_base.h"

RefPtr<BufferedWavPlayer> current_player_;
Effect* now_playing_;
Effect* play_next_;
class AnswerEffectGroup;
AnswerEffectGroup* current_question_;

#define COMMA ,

#define SUBNAMES(PREFIX, X, SEPARATOR)	\
  X(PREFIX##base)				\
  SEPARATOR					\
  X(PREFIX##answer1)				\
  SEPARATOR					\
  X(PREFIX##answer2)				\
  SEPARATOR					\
  X(PREFIX##answer3)				\
  SEPARATOR					\
  X(PREFIX##answer4)				\
  SEPARATOR					\
  X(PREFIX##answer5)				\
  SEPARATOR					\
  X(PREFIX##answer6)				\
  SEPARATOR					\
  X(PREFIX##answer7)				\
  SEPARATOR					\
  X(PREFIX##answer8)

#define MKNAMES(X) X, X "a1", X "a2", X "a3", X "a4", X "a5", X "a6", X "a7", X "a8"

#define JUST_NAME(X) X##_name
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

class AnswerEffectGroup {
public:
  AnswerEffectGroup(SUBNAMES(effect_, CONST_CHAR_NAME, COMMA)) : SUBNAMES(effect_, INIT_EFFECT, COMMA) {}

  SUBNAMES(effect_, DECLARE_EFFECT, );

  bool Activate() {
    if (!effect_base_) return false;
    Play(&effect_base_);
    if (effect_answer1_ || effect_answer2_ || effect_answer3_ || effect_answer4_) {
      current_question_ = this;
    }
    return true;
  }

  bool Answer(enum BUTTON button) {
    switch (button) {
      case BUTTON_TRIGGER_ONE:
	if (effect_answer1_) {
	  // Repeat question, so we're still in the same question mode.
	  Play(&effect_answer1_);
	  return true;
	}
	break;
      case BUTTON_TRIGGER_TWO:
	if (effect_answer2_) {
	  Play(&effect_answer2_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_THREE:
	if (effect_answer3_) {
	  Play(&effect_answer3_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_FOUR:
	if (effect_answer4_) {
	  Play(&effect_answer4_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_FIVE:
	if (effect_answer5_) {
	  Play(&effect_answer5_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_SIX:
	if (effect_answer6_) {
	  Play(&effect_answer6_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_SEVEN:
	if (effect_answer7_) {
	  Play(&effect_answer7_);
	  current_question_ = nullptr;
	  return true;
	}
	break;
      case BUTTON_TRIGGER_EIGHT:
	// Button 8 always cancels, but it can play a sound first.
	if (effect_answer8_) {
	  Play(&effect_answer8_);
	}
	current_question_ = nullptr;
	return true;
	
      default: break;
    }
    return false;
  }
};

class TriggerEffectGroup {
public:
  TriggerEffectGroup(SUBNAMES(clk, CONST_CHAR_NAME, COMMA),
		     SUBNAMES(hld, CONST_CHAR_NAME, COMMA)) :
                     clk(SUBNAMES(clk, JUST_NAME, COMMA)),
		     hld(SUBNAMES(hld, JUST_NAME, COMMA))
		     {}

  AnswerEffectGroup clk;
  AnswerEffectGroup hld;
  
  bool Event(EVENT event) {
    if (event == EVENT_FIRST_SAVED_CLICK_SHORT) {
      return clk.Activate();
    }
    if (event == EVENT_HELD_LONG) {
      return hld.Activate();
    }
    return false;
  }
};

#define DEFINE_TRIGGER(X) \
  TriggerEffectGroup trigger##X(MKNAMES("t" #X "clk"), MKNAMES("t" #X "hld"))

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
