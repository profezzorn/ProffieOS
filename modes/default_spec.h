#ifndef MODES_DEFAULT_SPEC_H
#define MODES_DEFAULT_SPEC_H

#include "menu_base.h"
#include "settings_menues.h"
#include "top_menu_wrapper.h"

template<class SPEC>
struct DefaultMenuSpec {
  typedef mode::SelectCancelMode SelectCancelMode;
  typedef mode::SteppedModeBase<SPEC> SteppedModeBase;
  typedef mode::SteppedMode<SPEC> SteppedMode;
  typedef mode::MenuBase<SPEC> MenuBase;
  typedef mode::SmoothWraparoundMode<SPEC> SmoothWraparoundMode;
  typedef mode::SmoothMode<SPEC> SmoothMode;
  typedef mode::SmoothVariationMode<SPEC> SmoothVariationMode;
  typedef mode::SteppedVariationMode<SPEC> SteppedVariationMode;
  typedef mode::ColorChangeMode<SPEC> ColorChangeMenu;

  typedef SoundLibraryV2 SoundLibrary;
  typedef mode::SmoothVolumeMode<SPEC> ChangeVolumeMode;

#ifdef DYNAMIC_BLADE_LENGTH  
  typedef mode::ShowLengthStyle<SPEC> ShowLengthStyle;
  typedef mode::ChangeBladeLengthBlade1<SPEC> ChangeBladeLengthMode;
#endif
#ifdef DYNAMIC_CLASH_THRESHOLD
  typedef mode::SmoothClashThresholdMode<SPEC> ChangeClashThresholdMode;
#endif
#ifdef DYNAMIC_BLADE_DIMMING
  typedef mode::SmoothChangeBladeDimmingMode<SPEC> ChangeBladeDimmingMode;
#endif
  typedef mode::SettingsMenuMode<SPEC> SettingsMenu;
  typedef mode::SelectFontMode<SPEC> SelectFont;
  typedef mode::SelectTrackMode<SPEC> SelectTrack;
  typedef mode::MovePresetUpEntry<SPEC> MovePresetUpEntry;
  typedef mode::MovePresetDownEntry<SPEC> MovePresetDownEntry;
  typedef mode::SelectPresetEntry<SPEC> SelectPresetEntry;
  typedef mode::InsertSelectedPresetEntry<SPEC> InsertSelectedPresetEntry;
  typedef mode::DeletePresetSubMenuEntry<SPEC> DeletePresetSubMenuEntry;

  typedef mode::EditStyleMenu<SPEC> EditStyleMenu;
  typedef mode::SelectArgMode<SPEC> EditStyleOptions;
  
  typedef mode::SelectStyleEntry<SPEC> SelectStyleEntry;
  typedef mode::ApplyColorsFromSelectedStyleEntry<SPEC> ApplyColorsFromSelectedStyleEntry;
  typedef mode::ApplyStyleArumentsFromSelectedStyleEntry<SPEC> ApplyStyleArumentsFromSelectedStyleEntry;
  typedef mode::ApplyColorsToAllBladesEntry<SPEC> ApplyColorsToAllBladesEntry;
  typedef mode::ResetColorsEntry<SPEC> ResetColorsEntry;
  typedef mode::ResetStyleArgumentsEntry<SPEC> ResetStyleArgumensEntry;
  typedef mode::SelectStyleMenu<SPEC> SelectStyleMenu;
  typedef mode::SelectFromListModeBase<SPEC> SelectFromListModeBase;

  typedef mode::SelectArgSmoothMode<SPEC> SelectArgSmooth;
  typedef mode::SelectArgTime<SPEC> SelectArgTime;
  typedef mode::SelectArgNumber<SPEC> SelectArgNumber;
  typedef mode::ColorSelectMode<SPEC> SelectArgColor;

  typedef mode::FileColorMenu<SPEC> FileColorMenu;
  typedef mode::ColorHueMode<SPEC> ColorHueMode;
  typedef mode::ColorBrightnessMode<SPEC> ColorBrightnessMode;
  typedef mode::ColorRedMode<SPEC> ColorRedMode;
  typedef mode::ColorGreenMode<SPEC> ColorGreenMode;
  typedef mode::ColorBlueMode<SPEC> ColorBlueMode;
  typedef mode::SelectColorEntry<SPEC> SelectColorEntry;
  typedef mode::UseSelectedColorEntry<SPEC> UseSelectedColorEntry;
  typedef mode::ResetColorToDefaultEntry<SPEC> ResetColorToDefaultEntry;
  typedef mode::ColorGammaMode<SPEC> GammaMode;

  typedef mode::EditPresetMenu<SPEC> EditPresetsMenu;
//  typedef mode::TopMenuWrapper<SPEC, mode::BasicTopMenu<SPEC>> TopMenu;
  typedef mode::BasicTopMenu<SPEC> TopMenu;
};

// This spec goes directly to the settings menu with no edit mode options.
template<class SPEC>
struct SettingsOnlyMenuSpec : public DefaultMenuSpec<SPEC> {
  typedef mode::SettingsMenuMode<SPEC> TopMenu;
};


#endif
