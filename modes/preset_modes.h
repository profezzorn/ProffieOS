#ifndef MODE_PRESET_MODES
#define MODE_PRESET_MODES

#include "../common/bytearray.h"
#include "menu_list.h"

// Returns the position of the current preset.
int prop_GetPresetPosition();
void prop_MovePreset(int position);
int GetTotalPresets();

namespace mode {

template<class SPEC, class ACTION_ENTRY>
class  ConfirmMenu : public MenuEntryMenu<SPEC,
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>,
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>,
  ACTION_ENTRY,
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>,
  PopMenuEntry<typename SPEC::SoundLibrary::tCancel>
> {};

template<class SPEC, class CMD, class ARG, class SOUND>
using ConfirmCommandMenuEntry =
  SubMenuEntry<ConfirmMenu<SPEC, CommandMenuEntry<CMD, ARG, SOUND>>, SOUND>;

int menu_selected_preset = -1;

template<class SPEC>
struct MovePresetUpEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>()->SayMovePresetUp();
  }
  void select(int entry) override {
    menu_selected_preset = -1;
    int pos = prop_GetPresetPosition();
    if (pos > 0) {
      prop_MovePreset(pos - 1);
      getSL<SPEC>()->SaySelect();
    } else {
      getSL<SPEC>()->SayListEndBuzz();
    }
  }
};
  
template<class SPEC>
struct MovePresetDownEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>()->SayMovePresetDown();
  }
  void select(int entry) override {
    menu_selected_preset = -1;
    int pos = prop_GetPresetPosition();
    // Check if this is the last preset.
    CurrentPreset tmp;
    tmp.SetPreset(pos + 1);
    if (tmp.preset_num == pos + 1) {
      prop_MovePreset(pos + 1);
      getSL<SPEC>()->SaySelect();
    } else {
      getSL<SPEC>()->SayListEndBuzz();
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
    int pos = prop_GetPresetPosition();
    if (pos != 0) {
      prop_MovePreset(0);
      getSL<SPEC>->SaySelect();
    } else {
      getSL<SPEC>->SayListEndBuzz();
    }
  }
};
  
template<class SPEC>
struct SelectPresetEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>()->SaySelectPreset();
  }
  void select(int entry) override {
    menu_selected_preset = prop_GetPresetPosition();
    getSL<SPEC>()->SaySelect();
  }
};
  
template<class SPEC>
struct InsertSelectedPresetEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>()->SaySelectPreset();
  }
  void select(int entry) override {
    int from_pos = menu_selected_preset;
    if (from_pos == -1) {
      getSL<SPEC>()->SayNoPresetSelected();
      return;
    }
    int to_pos = prop_GetPresetPosition();
    CurrentPreset preset;
    preset.SetPreset(from_pos);
    preset.preset_num = -1000;
    preset.SaveAt(to_pos);
    char p[20];
    itoa(to_pos, p, 10);
    CommandParser::DoParse("set_preset", p);
    if (to_pos <= menu_selected_preset) {
      menu_selected_preset++;
    }
    getSL<SPEC>()->SayPresetInserted();
  }
};


template<class SPEC, int BLADE>
struct EditBladeSubMenuEntry : public MenuEntry {
  void say(int entry) override {
    getSL<SPEC>()->SaySelectPreset();
  }
  void select(int entry) override {
    menu_current_blade = BLADE;
    pushMode<typename SPEC::EditStyleMenu>();
  }
};

#define EDIT_BLADE_SUBMENU(N) EditBladeSubMenuEntry<SPEC, N>,
  
template<class SPEC>
class EditPresetMenu : public MenuEntryMenu<SPEC,
  SubMenuEntry<typename SPEC::SelectFont, typename SPEC::SoundLibrary::tEditFont>,
  SubMenuEntry<typename SPEC::SelectTrack, typename SPEC::SoundLibrary::tEditTrack>,
  ONCEPERBLADE(EDIT_BLADE_SUBMENU)
  typename SPEC::MovePresetUpEntry,
  typename SPEC::MovePresetDownEntry,
  typename SPEC::SelectPresetEntry,
  typename SPEC::InsertSelectedPresetEntry,
  ConfirmCommandMenuEntry<SPEC, STRTYPE("delete_preset"), STRTYPE(""), typename SPEC::SoundLibrary::tDeletePreset>
> {};
  
}  // namespace mode

#endif
