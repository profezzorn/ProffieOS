#ifndef MODE_PRESET_MODES
#define MODE_PRESET_MODES

#include "../common/bytearray.h"

// Returns the position of the current preset.
int GetPresetPosition();
int MovePreset(int position);
int GetTotalPresets();
int CopyPreset(int from_position);

namespace mode {

template<class SPEC, class ACTION_ENTRY>
using ConfirmMenu = typename SPEC::template MenuListMode<
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>,
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>,
  ACTION_ENTRY,
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>,
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>>;

template<class SPEC, class CMD, class ARG, class SOUND>
using ConfirmCommandMenuEntry =
  SubMenuEntry<ConfirmMenu<SPEC, CommandMenuEntry<CMD, ARG, SOUND>>, SOUND>;

int menu_selected_preset = -1;

template<class SPEC>
struct MovePresetUpEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>->SayMovePresetUp();
  }
  void select(int entry) override {
    menu_selected_preset = -1;
    int pos = GetPresetPosition();
    if (pos > 0) {
      MovePreset(pos - 1);
      getSL<SPEC>->SaySelect();
    } else {
      getSL<SPEC>->SayListEndBuzz();
    }
  }
};
  
template<class SPEC>
struct MovePresetDownEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>->SayMovePresetDown();
  }
  void select(int entry) override {
    menu_selected_preset = -1;
    int pos = GetPresetPosition();
    int num_presets = GetTotalPresets();
    if (pos + 1 < num_presets) {
      MovePreset(pos + 1);
      getSL<SPEC>->SaySelect();
    } else {
      getSL<SPEC>->SayListEndBuzz();
    }
  }
};
  
template<class SPEC>
struct MovePresetToBeginningEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>->SayMovePresetToBeginning();
  }
  void select(int entry) override {
    menu_selected_preset = -1;
    int pos = GetPresetPosition();
    if (pos != 0) {
      MovePreset(0);
      getSL<SPEC>->SaySelect();
    } else {
      getSL<SPEC>->SayListEndBuzz();
    }
  }
};
  
template<class SPEC>
struct SelectPresetEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>->SaySelectPreset();
  }
  void select(int entry) override {
    menu_selected_preset = GetPresetPosition();
    getSL<SPEC>->SaySelect();
  }
};
  
template<class SPEC>
struct InsertSelectedPresetEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>->SaySelectPreset();
  }
  void select(int entry) override {
    int pos = GetPresetPosition();
    CopyPreset(menu_selected_preset);
    if (pos <= menu_selected_preset) {
      menu_selected_preset++;
    }
    getSL<SPEC>->SayPresetInserted();
  }
};


template<class SPEC, int BLADE>
struct EditBladeSubMenuEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>->SaySelectPreset();
  }
  void select(int entry) override {
    menu_current_blade = BLADE;
    pushMode<SPEC::EditStyleMenu>();
  }
};

#define EDIT_BLADE_SUBMENU(N) EditBladeSubMenuEntry<SPEC, N>,
  
template<class SPEC>
using EditPresentMenu = typename SPEC::template MenuListMode<
  SubMenuEntry<typename SPEC::SelectFont, typename SPEC::SoundLibrary::tEditFont>,
  SubMenuEntry<typename SPEC::SelectTrack, typename SPEC::SoundLibrary::tEditTrack>,
  ONCEPERBLADE(EDIT_BLADE_SUBMENU)
  SPEC::MovePresetUpEntry,
  SPEC::MovePresetDownEntry,
  SPEC::SelectPresetEntry,
  SPEC::InsertSelectedPreset,
  ConfirmCommandMenuEntry<SPEC, STRTYPE("delete_preset"), STRTYPE(""), typename SPEC::SoundLibrary::tDeletePreset>
  >;
  
}  // namespace mode

#endif
