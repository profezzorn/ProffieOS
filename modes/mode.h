#ifndef MODES_MODE_H
#define MODES_MODE_H

// Basic mode interface
// allows you to temporarily override buttons and other types of events.
struct ModeInterface {
  virtual void mode_activate(bool onreturn) {}
  virtual bool mode_Event2(enum BUTTON button, EVENT event, uint32_t modifiers) {
    return false;
  }
  virtual void mode_Loop() {}
  virtual bool mode_Parse(const char *cmd, const char* arg) { return false; }

  ModeInterface* previous_;
};

ModeInterface* current_mode;

template<class MODE>
void pushMode() {
  ModeInterface* mode = getPtr<MODE>();
  mode->previous_ = current_mode;
  current_mode = mode;
  mode->mode_activate(false);
}

void popMode() {
  current_mode = current_mode->previous_;
  current_mode->mode_activate(true);
}

namespace mode {
template<class SPEC> typename SPEC::SoundLibrary* getSL() {
  return getPtr<typename SPEC::SoundLibrary>();
}

}  // namespace mode

#endif
