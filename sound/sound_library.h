#ifndef SOUND_SOUND_LIBRARY_H
#define SOUND_SOUND_LIBRARY_H

#include "sound_queue.h"

// Sound library
EFFECT(mnum); // menu numbers

class VoicePackVersionFile : public ConfigFile {
public:
  void iterateVariables(VariableOP *op) override {
    CONFIG_VARIABLE2(voice_pack_version, 1);
  }
  int voice_pack_version;  
};

// Sound library
enum SayType {
  SAY_DECIMAL,
  SAY_WHOLE,
};

// New colors should be added at end of enum and assigned numbers for each COLOR_ should not be changed.
enum ColorNumber {
  COLOR_RED = 1,
  COLOR_ORANGERED = 2,
  COLOR_DARKORANGE = 3,
  COLOR_ORANGE = 4,
  COLOR_GOLD = 5,
  COLOR_YELLOW = 6,
  COLOR_GREENYELLOW = 7,
  COLOR_GREEN = 8,
  COLOR_AQUAMARINE = 9,
  COLOR_CYAN = 10,
  COLOR_DEEPSKYBLUE = 11,
  COLOR_DODGERBLUE = 12,
  COLOR_BLUE = 13,
  COLOR_ICEBLUE = 14,
  COLOR_INDIGO = 15,
  COLOR_PURPLE = 16,
  COLOR_DEEPPURPLE = 17,
  COLOR_MAGENTA = 18,
  COLOR_DEEPPINK = 19,
  COLOR_SILVER = 20,
  COLOR_GLACIER = 21,
  COLOR_ICEWHITE = 22,
  COLOR_LIGHTCYAN = 23,   
  COLOR_MOCCASIN = 24,
  COLOR_LEMONCHIFFON = 25,
  COLOR_NAVAJOWHITE = 26,
  COLOR_WHITE = 27,
};

class SoundQueueSingleton : public SoundQueue<16>, Looper, SaberBase {
public:
  const char* name() { return "SoundQueue"; }
  void Loop() override {
    PollSoundQueue(wav_player_);
  }

  void require_version(int version) {
    required_version_ = std::max(version, required_version_);
  }

  void SB_Effect(EffectType effect, EffectLocation location) override {
    if (effect == EFFECT_CHDIR) CheckVersion();
  }
private:
  void CheckVersion() {
    int found_version = 0;
    if (SFX_mnum) {
      VoicePackVersionFile f;
      f.ReadInCurrentDir("voicepack.ini");
      found_version = f.voice_pack_version;
      PVLOG_STATUS << "Sound library version " << found_version << " found.\n";
    }
    if (found_version < required_version_) {
      PVLOG_ERROR << "ERROR - Sound library version " << required_version_ << " required.\n";
      ProffieOSErrors::error_in_font_directory(); // Make new error for voice pack?
    }
  }

  int required_version_ = 0;
  RefPtr<BufferedWavPlayer> wav_player_;
};

#define SOUNDQ (getPtr<SoundQueueSingleton>())

class SoundLibrary  {
public:
  static const int SoundLibraryVersion = 1;
  static bool Play(const char* f) { return SOUNDQ->Play(f); }
  static bool Play(SoundToPlay p) { return SOUNDQ->Play(p); }
  static void fadeout(float len) { return SOUNDQ->fadeout(len); }
  static bool busy() { return SOUNDQ->busy(); }
  static void Poll(RefPtr<BufferedWavPlayer>& player) {}

  static void init() {
    SOUNDQ->require_version(SoundLibraryVersion);
  }

  void SayWhole(int number) {
    if (number == 0) {
      Say0();
      return;
    }
    int thousands = number / 1000;
    if (thousands) {
      SayWhole(thousands);
      Say1000();
      number %= 1000;
    }
    int hundreds = number / 100;
    if (hundreds) {
      SayWhole(hundreds);
      Say100();
      number %= 100;
    }
    int tens = number / 10;
    number %= 10;
    switch (tens) {
      default:
        number += tens * 10;
          break;
      case 2: Say20(); break;
      case 3: Say30(); break;
      case 4: Say40(); break;
      case 5: Say50(); break;
      case 6: Say60(); break;
      case 7: Say70(); break;
      case 8: Say80(); break;
      case 9: Say90(); break;
    }
    if (number) {
      Play(SoundToPlay(&SFX_mnum, number - 1));
    }
  }

  void SayNumber(float number, SayType say_type) {
    SayWhole((int)floorf(number));
    switch (say_type) {
      case SAY_DECIMAL: {
        int hundredths = (int)floorf(fract(number) * 100);
         SayPoint();
         SayWhole(hundredths / 10);
         SayWhole(hundredths % 10);
        break;
      }
      default:
        break;
    }
  }

  void SayBatteryVolts() {
    SayBatteryLevel();
    SayNumber(battery_monitor.battery(), SAY_DECIMAL);
    SayVolts();
  }
        
  void SayBatteryPercent() {
    SayBatteryLevel();
    SayNumber(battery_monitor.battery_percent(), SAY_WHOLE);
    SayPercent();
  }

#define ADD_SL_SOUND(NAME, BASE)                                        \
  void Say##NAME() { SOUNDQ->Play(BASE ".wav"); }                       \
  /* t for "trampoline" */                                              \
  struct t##NAME { static void say() { SOUNDQ->Play(BASE ".wav"); } }
  
  ADD_SL_SOUND(Red, "clrlst/clrlst01");
  ADD_SL_SOUND(OrangeRed, "clrlst/clrlst02");
  ADD_SL_SOUND(DarkOrange, "clrlst/clrlst03");
  ADD_SL_SOUND(Orange, "clrlst/clrlst04");
  ADD_SL_SOUND(Gold, "clrlst/clrlst05");
  ADD_SL_SOUND(Yellow, "clrlst/clrlst06");
  ADD_SL_SOUND(GreenYellow, "clrlst/clrlst07");
  ADD_SL_SOUND(Green, "clrlst/clrlst08");
  ADD_SL_SOUND(AquaMarine, "clrlst/clrlst09");
  ADD_SL_SOUND(Cyan, "clrlst/clrlst10");
  ADD_SL_SOUND(DeepSkyBlue, "clrlst/clrlst11");
  ADD_SL_SOUND(DodgerBlue, "clrlst/clrlst12");
  ADD_SL_SOUND(Blue, "clrlst/clrlst13");
  ADD_SL_SOUND(IceBlue, "clrlst/clrlst14");
  ADD_SL_SOUND(Indigo, "clrlst/clrlst15");
  ADD_SL_SOUND(Purple, "clrlst/clrlst16");
  ADD_SL_SOUND(DeepPurple, "clrlst/clrlst17");
  ADD_SL_SOUND(Magenta, "clrlst/clrlst18");
  ADD_SL_SOUND(DeepPink, "clrlst/clrlst19");
  ADD_SL_SOUND(Silver, "clrlst/clrlst20");
  ADD_SL_SOUND(Glacier, "clrlst/clrlst21");
  ADD_SL_SOUND(IceWhite, "clrlst/clrlst22");
  ADD_SL_SOUND(LightCyan, "clrlst/clrlst23");
  ADD_SL_SOUND(Moccasin, "clrlst/clrlst24");
  ADD_SL_SOUND(LemonChiffon, "clrlst/clrlst25");
  ADD_SL_SOUND(NavajoWhite, "clrlst/clrlst26");
  ADD_SL_SOUND(White, "clrlst/clrlst27");

  ADD_SL_SOUND(0, "mzero");
  ADD_SL_SOUND(1, "mnum/mnum1");
  ADD_SL_SOUND(2, "mnum/mnum2");
  ADD_SL_SOUND(3, "mnum/mnum3");
  ADD_SL_SOUND(4, "mnum/mnum4");
  ADD_SL_SOUND(5, "mnum/mnum5");
  ADD_SL_SOUND(6, "mnum/mnum6");
  ADD_SL_SOUND(7, "mnum/mnum7");
  ADD_SL_SOUND(8, "mnum/mnum8");
  ADD_SL_SOUND(9, "mnum/mnum9");
  ADD_SL_SOUND(10, "mnum/mnum10");
  ADD_SL_SOUND(11, "mnum/mnum11");
  ADD_SL_SOUND(12, "mnum/mnum12");
  ADD_SL_SOUND(13, "mnum/mnum13");
  ADD_SL_SOUND(14, "mnum/mnum14");
  ADD_SL_SOUND(15, "mnum/mnum15");
  ADD_SL_SOUND(16, "mnum/mnum16");
  ADD_SL_SOUND(17, "mnum/mnum17");
  ADD_SL_SOUND(18, "mnum/mnum18");
  ADD_SL_SOUND(19, "mnum/mnum19");
  ADD_SL_SOUND(20, "mnum/mnum20");
  ADD_SL_SOUND(30, "thirty");
  ADD_SL_SOUND(40, "forty");
  ADD_SL_SOUND(50, "fifty");
  ADD_SL_SOUND(60, "sixty");
  ADD_SL_SOUND(70, "seventy");
  ADD_SL_SOUND(80, "eighty");
  ADD_SL_SOUND(90, "ninety");
  ADD_SL_SOUND(100, "hundred");
  ADD_SL_SOUND(1000, "thousand");
  
  ADD_SL_SOUND(Point, "mpoint");
  
  ADD_SL_SOUND(Accept, "maccept");
  ADD_SL_SOUND(AdjustBlackLevel, "mblack");
  ADD_SL_SOUND(AdjustColorHue, "mhue");
  ADD_SL_SOUND(AdjustWhiteLevel, "mwhite");
  ADD_SL_SOUND(AltColor, "malt");
  ADD_SL_SOUND(Auto, "mauto");
  ADD_SL_SOUND(BaseColor, "mbase");
  ADD_SL_SOUND(BatteryLevel, "mbatt");
  ADD_SL_SOUND(BlastColor, "mblast");
  ADD_SL_SOUND(Cancel, "mcancel");
  ADD_SL_SOUND(ChoreographyBegin, "chreobgn");
  ADD_SL_SOUND(ChoreographyEnd, "chreoend");
  ADD_SL_SOUND(ClashColor, "mclash");
  ADD_SL_SOUND(ClashDetectionLevel, "mbmclash");
  ADD_SL_SOUND(ClashLockupPosition, "mlockpos");
  ADD_SL_SOUND(ColorList, "mcolorlt");
  ADD_SL_SOUND(ColorMenu, "mcolorsb");
  ADD_SL_SOUND(ColorOptions, "mcolorop");
  ADD_SL_SOUND(Confirm, "mconfirm");
  ADD_SL_SOUND(ConfirmSelection, "maffirm");
  ADD_SL_SOUND(CoolDownOptions, "mcooldwn");
  ADD_SL_SOUND(CopyColors, "mcopyc");
  ADD_SL_SOUND(CopyPreset, "mcopy");
  ADD_SL_SOUND(DeletePreset, "mdelete");
  ADD_SL_SOUND(Disabled, "mfalse");
  ADD_SL_SOUND(Down, "mdown"); // Sound for decrease
  ADD_SL_SOUND(DragColor, "mdrag");
  ADD_SL_SOUND(DragSize, "mdragsz");
  ADD_SL_SOUND(EditBladeLength, "mlength");
  ADD_SL_SOUND(EditBrightness, "mdim");
  ADD_SL_SOUND(EditClashThreshold, "mclashth");
  ADD_SL_SOUND(EditColor, "mcolor");
  ADD_SL_SOUND(EditControlSettings, "mcontrol");
  ADD_SL_SOUND(EditFont, "mfont");
  ADD_SL_SOUND(EditMode, "medit");   // 1-sec leading silence delay
  ADD_SL_SOUND(EditSettings, "msetting");    // 1-sec leading silence delay
  ADD_SL_SOUND(EditStyle, "mstyle");
  ADD_SL_SOUND(EditStyleSettings, "mstylset");
  ADD_SL_SOUND(EditTrack, "mtrack");
  ADD_SL_SOUND(EditVolume, "mvolume");
  ADD_SL_SOUND(EmitterColor, "memitter");
  ADD_SL_SOUND(EmitterSize, "memitsz");
  ADD_SL_SOUND(Enabled, "mtrue");
  ADD_SL_SOUND(EnterVolumeMenu, "vmbegin");
  ADD_SL_SOUND(Exit, "mexit");
  ADD_SL_SOUND(ForcePush, "mpush");
  ADD_SL_SOUND(ForcePushLength, "mpushlen");
  ADD_SL_SOUND(GestureMenu, "mgestsub");
  ADD_SL_SOUND(GesturesOff, "mgestoff");
  ADD_SL_SOUND(GesturesOn, "mgeston");
  ADD_SL_SOUND(IgnitionColor, "mignite");
  ADD_SL_SOUND(IgnitionDelay, "migdelay");
  ADD_SL_SOUND(IgnitionOptions, "migopt");
  ADD_SL_SOUND(IgnitionTime, "mouttime");
  ADD_SL_SOUND(KeepRehearsal, "rehrsold");
  ADD_SL_SOUND(LightningBlockColor, "mlb");
  ADD_SL_SOUND(LockupColor, "mlockup");
  ADD_SL_SOUND(LockupDelay, "mlockdly");
  ADD_SL_SOUND(Loop, "mloop");
  ADD_SL_SOUND(MainMenu, "mmain");
  ADD_SL_SOUND(Maximum, "mmax");
  ADD_SL_SOUND(MaximumClashStrength, "maxclash");
  ADD_SL_SOUND(MaximumVolume, "volmax");
  ADD_SL_SOUND(MeltSize, "mmeltsz");
  ADD_SL_SOUND(Millis, "mmillis");
  ADD_SL_SOUND(Minimum, "mmin");
  ADD_SL_SOUND(MinimumVolume, "volmin");
  ADD_SL_SOUND(NoChoreographyAvailable, "nochreo");
  ADD_SL_SOUND(OffColor, "moff");
  ADD_SL_SOUND(OffOption, "moffopt");
  ADD_SL_SOUND(Option, "moptnum");
  ADD_SL_SOUND(Percent, "mpercent");
  ADD_SL_SOUND(PostOffColor, "mpstoff");
  ADD_SL_SOUND(PowerLock, "mpwrlock");
  ADD_SL_SOUND(PowerUpOptions, "mpwrup");
  ADD_SL_SOUND(PreonColor, "mpreon");
  ADD_SL_SOUND(PreonOptions, "mpreopt");
  ADD_SL_SOUND(PreonSize, "mpreonsz");
  ADD_SL_SOUND(Random, "mrandom");
  ADD_SL_SOUND(RehearseBegin, "rehrsbgn");
  ADD_SL_SOUND(RehearseEnd, "rehrsend");
  ADD_SL_SOUND(RehearseNew, "rehrsnew"); // rename?
  ADD_SL_SOUND(ResetColors, "mresetc");
  ADD_SL_SOUND(RetractionColor, "mretract");
  ADD_SL_SOUND(RetractionDelay, "mrtdelay");
  ADD_SL_SOUND(RetractionOptions, "mrtopt");
  ADD_SL_SOUND(RetractionTime, "mintime");
  ADD_SL_SOUND(Revert, "mrevert");
  ADD_SL_SOUND(Rotate, "mrotate");
  ADD_SL_SOUND(Save, "msave");
  ADD_SL_SOUND(Select, "mselect"); // Sound for OK / Affirmative / Enter
  ADD_SL_SOUND(SelectBlade, "mblade");
  ADD_SL_SOUND(SelectEffect, "meffect");
  ADD_SL_SOUND(SelectOption, "moption");
  ADD_SL_SOUND(SelectPreset, "mpreset");
  ADD_SL_SOUND(SelectStyle, "mstylsel");
  ADD_SL_SOUND(SettingsMenu, "msetsub");
  ADD_SL_SOUND(StabColor, "mstab");
  ADD_SL_SOUND(StabIgnition, "mstabon");
  ADD_SL_SOUND(Style, "mstylnum");
  ADD_SL_SOUND(StyleMenu, "mstylesb");
  ADD_SL_SOUND(StyleOptions, "mstylopt");
  ADD_SL_SOUND(StyleSettings, "stylstm");
  ADD_SL_SOUND(SwingColor, "mswing");
  ADD_SL_SOUND(SwingIgnition, "mswingon");
  ADD_SL_SOUND(SwingOnSpeed, "mswingsp");
  ADD_SL_SOUND(SwingOption, "mswngopt");
  ADD_SL_SOUND(TheBatteryLevelIs, "battlevl");
  ADD_SL_SOUND(ThrustIgnition, "mthrston");
  ADD_SL_SOUND(TwistIgnition, "mtwiston");
  ADD_SL_SOUND(TwistRetraction, "mtwstoff");
  ADD_SL_SOUND(Up, "mup"); // Sound for increase
  ADD_SL_SOUND(Volts, "mvolts");
  ADD_SL_SOUND(VolumeDown, "voldown"); // Sound for decreasing volume
  ADD_SL_SOUND(VolumeMenuEnd, "vmend");
  ADD_SL_SOUND(VolumeUp, "volup"); // Sound for increasing volume
  ADD_SL_SOUND(ZoomingIn, "mzoom"); // Sound for color menu "zooming in"

  void SayColor(ColorNumber n) {
    switch (n) {
    case COLOR_RED: SayRed(); break;
    case COLOR_ORANGERED: SayOrangeRed(); break;
    case COLOR_DARKORANGE: SayDarkOrange(); break;
    case COLOR_ORANGE: SayOrange(); break;
    case COLOR_GOLD: SayGold(); break;
    case COLOR_YELLOW: SayYellow(); break;
    case COLOR_GREENYELLOW: SayGreenYellow(); break;
    case COLOR_GREEN: SayGreen(); break;
    case COLOR_AQUAMARINE: SayAquaMarine(); break;
    case COLOR_CYAN: SayCyan(); break;
    case COLOR_DEEPSKYBLUE: SayDeepSkyBlue(); break;
    case COLOR_DODGERBLUE: SayDodgerBlue(); break;
    case COLOR_BLUE: SayBlue(); break;
    case COLOR_ICEBLUE: SayIceBlue(); break;
    case COLOR_INDIGO: SayIndigo(); break;
    case COLOR_PURPLE: SayPurple(); break;
    case COLOR_DEEPPURPLE: SayDeepPurple(); break;
    case COLOR_MAGENTA: SayMagenta(); break;
    case COLOR_DEEPPINK: SayDeepPink(); break;
    case COLOR_SILVER: SaySilver(); break;
    case COLOR_GLACIER: SayGlacier(); break;
    case COLOR_ICEWHITE: SayIceWhite(); break;
    case COLOR_LIGHTCYAN: SayLightCyan(); break;
    case COLOR_MOCCASIN: SayMoccasin(); break;
    case COLOR_LEMONCHIFFON: SayLemonChiffon(); break;
    case COLOR_NAVAJOWHITE: SayNavajoWhite(); break;
    case COLOR_WHITE: SayWhite(); break;
    }
  }

  void SayBool(bool v) {
    if (v) {
      SayEnabled();
    } else {
      SayDisabled(); 
    }
  }

 void SayArgument(int argument) {
    switch (argument) {
      case BASE_COLOR_ARG: // 1
        SayBaseColor();
        break;
      case ALT_COLOR_ARG: // 2
        SayAltColor();
        break;
      case STYLE_OPTION_ARG: // 3
        SayStyleOptions();
        break;
      case IGNITION_OPTION_ARG: // 4
        SayIgnitionOptions();
        break;
      case IGNITION_TIME_ARG: // 5
        SayIgnitionTime();
        break;
      case IGNITION_DELAY_ARG:  // 6
        SayIgnitionDelay();
        break;
      case IGNITION_COLOR_ARG:  // 7
        SayIgnitionColor();
        break;
      case IGNITION_POWER_UP_ARG:  // 8
        SayPowerUpOptions();
        break;
      case BLAST_COLOR_ARG:  // 9
        SayBlastColor();
        break;
      case CLASH_COLOR_ARG: // 10
        SayClashColor();
        break;
      case LOCKUP_COLOR_ARG: // 11
        SayLockupColor();
        break;
      case LOCKUP_POSITION_ARG:  // 12
        SayClashLockupPosition();
        break;
      case DRAG_COLOR_ARG:  // 13
        SayDragColor();
        break;
      case DRAG_SIZE_ARG:  // 14
        SayDragSize();
        break;
      case LB_COLOR_ARG:  // 15
        SayLightningBlockColor();
        break;
      case STAB_COLOR_ARG:  // 16
        SayStabColor();
        break;
      case MELT_SIZE_ARG:  // 17
        SayMeltSize();
        break;
      case SWING_COLOR_ARG:  // 18
        SaySwingColor();
        break;
      case SWING_OPTION_ARG:  // 19
        SaySwingOption();
        break;
      case EMITTER_COLOR_ARG: // 20
        SayEmitterColor();
        break;
      case EMITTER_SIZE_ARG:  // 21
        SayEmitterSize();
        break;
      case PREON_COLOR_ARG:  // 22
        SayPreonColor();
        break;
      case PREON_OPTION_ARG:  // 23
        SayPreonOptions();
        break;
      case PREON_SIZE_ARG:  // 24
        SayPreonSize();
        break;
      case RETRACTION_OPTION_ARG:  // 25
        SayRetractionOptions();
        break;
      case RETRACTION_TIME_ARG:  // 26
        SayRetractionTime();
        break;
      case RETRACTION_DELAY_ARG:  // 27
        SayRetractionDelay();
        break;
      case RETRACTION_COLOR_ARG:  // 28
        SayRetractionColor();
        break;
      case RETRACTION_COOL_DOWN_ARG:  // 29
        SayCoolDownOptions();
        break;
      case POSTOFF_COLOR_ARG:  // 30
        SayPostOffColor();
        break;
      case OFF_COLOR_ARG: // 31
        SayOffColor();
        break;
      case OFF_OPTION_ARG:  // 32
        SayOffOption();
        break;
      case ALT_COLOR2_ARG: // 33
        SayAltColor();
        SayNumber(2, SAY_WHOLE);
        break;
      case ALT_COLOR3_ARG:  // 34
        SayAltColor();
        SayNumber(3, SAY_WHOLE);
        break;
      case STYLE_OPTION2_ARG: // 35
        SayStyleOptions();
        SayNumber(2, SAY_WHOLE);
        break;
      case STYLE_OPTION3_ARG:  // 36
        SayStyleOptions();
        SayNumber(3, SAY_WHOLE);
        break;
      case IGNITION_OPTION2_ARG: // 37
        SayIgnitionOptions();
        SayNumber(2, SAY_WHOLE);
        break;
      case RETRACTION_OPTION2_ARG:  // 38
        SayRetractionOptions();
        SayNumber(2, SAY_WHOLE);
        break;
      default:
        SayOption();
        SayWhole(argument);
    }
  }
};

class SoundLibraryV2 : public SoundLibrary {
public:
  static const int SoundLibraryVersion = 2;
  static void init() {
    SOUNDQ->require_version(SoundLibraryVersion);
  }

  ADD_SL_SOUND(AdjustRed, "madjred");
  ADD_SL_SOUND(AdjustBlue, "madjblue");
  ADD_SL_SOUND(AdjustGreen, "madjgren");
  ADD_SL_SOUND(AdjustSaturation, "madjsat");
  ADD_SL_SOUND(NoColorSelected, "mnoselcl");

  ADD_SL_SOUND(PasteColor, "mpastec");
  ADD_SL_SOUND(CopyColor, "mcopyc2");

  ADD_SL_SOUND(MovePresetUp, "mmpsetup");
  ADD_SL_SOUND(MovePresetDown, "mmpsetdn");
  ADD_SL_SOUND(MovePresetToBeginning, "mmpsetbg");
  ADD_SL_SOUND(PresetInserted, "mpsetins");
  ADD_SL_SOUND(Preset, "mpset");
  ADD_SL_SOUND(EditPresets, "medpsets");

//  ADD_SL_SOUND(EditStyleOptions, "mestyopt");

  ADD_SL_SOUND(InsertSelectedPreset, "minpset");
  ADD_SL_SOUND(MoveSelectedPreset, "mmpset");
  ADD_SL_SOUND(NoPresetSelected, "mnopsel");

  ADD_SL_SOUND(NoStyleSelected, "mnostsel");
  ADD_SL_SOUND(ApplyColorsFromSelectedStyle, "macolsty");
  ADD_SL_SOUND(ApplyStyleSettingsFromSelectedStyle, "maargsty");
  ADD_SL_SOUND(ApplyColorsToAllBlades, "macolall");
  ADD_SL_SOUND(ResetToDefaultColor, "mcolrst");

  ADD_SL_SOUND(ResetStyleSettings, "mrststys");

  ADD_SL_SOUND(Blade, "mbld");
  ADD_SL_SOUND(Seconds, "mseconds");

  ADD_SL_SOUND(SDAccess, "msdacc");
  ADD_SL_SOUND(Sequential, "msequent");

  // Sound used to indicate that there are no more options
  // or list entries.
  ADD_SL_SOUND(ListEndBuzz, "mbuzz");
  ADD_SL_SOUND(CantDeleteLastPreset, "mcantdlp");
  ADD_SL_SOUND(ThisStyleHasNoSettings, "mstnoset");
  ADD_SL_SOUND(EditSettingsV2, "mseting2");    // NO pause!
};

// Please don't forget to call sound_library_->init();
#define sound_library_ (*getPtr<SoundLibrary>())

// Please don't forget to call sound_librar_v2->init();
#define sound_library_v2 (*getPtr<SoundLibraryV2>())

#endif
