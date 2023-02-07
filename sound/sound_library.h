#ifndef SOUND_SOUND_LIBRARY_H
#define SOUND_SOUND_LIBRARY_H

// Sound library
EFFECT(mnum); // menu numbers
enum SayType {
  SAY_DECIMAL,
  SAY_WHOLE,
};

#ifdef SAY_COLOR_LIST
EFFECT(clrlst); // spoken color names for SAY_COLOR_LIST
#endif
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

class SoundLibrary {
public:
  SoundQueue<16> sound_queue_;
  void Play(const char* file) { sound_queue_.Play(file); }
  void Play(SoundToPlay stp) { sound_queue_.Play(stp); }
  void Poll(RefPtr<BufferedWavPlayer>& wav_player) {
    sound_queue_.PollSoundQueue(wav_player);
  }

  void TensValue(int number) {
    if (number <= 20) {
      // Reminder - use search and replace "Play(SoundToPlay())" per PR comments
      Play(SoundToPlay(&SFX_mnum, number - 1));
    } else {
      int tens = ((int)floorf(number / 10)) % 10;
      number -= (tens * 10);
      switch (tens) {
        default:
        case 2:
	  Play(SoundToPlay(&SFX_mnum, (tens * 10) - 1));
          break;
        case 3:
          Play("thirty.wav");
          break;
        case 4:
          Play("forty.wav");
          break;
        case 5:
	  Play("fifty.wav");
          break;
        case 6:
          Play("sixty.wav");
          break;
        case 7:
	  Play("seventy.wav");
          break;
        case 8:
	  Play("eighty.wav");
          break;
        case 9:
	  Play("ninety.wav");
          break;
      }
      if (number != 0) Play(SoundToPlay(&SFX_mnum, number - 1));
    }
  }

  void SayNumber(float number, SayType say_type) {
    int thousand = ((int)floorf(number / 1000)) % 100;
    int hundred = ((int)floorf(number / 100)) % 10;
    int ones = ((int)floorf(number)) % 100;
    int tenths = ((int)floorf(number * 10)) % 10;
    int hundredths = ((int)floorf(number * 100)) % 10;
    switch (say_type) {
       case SAY_DECIMAL:
        // Tens & Ones
        if (number == 0) {
	  Play(SoundToPlay("mzero.wav"));
        } else {
          TensValue(ones);
        }
        // Decimal / Point
	Play(SoundToPlay("mpoint.wav"));
        // Tenths
        if (tenths == 0) {
          Play("mzero.wav");
        } else {
          Play(SoundToPlay(&SFX_mnum, tenths - 1));
        }
        // Hundredths
        if (hundredths != 0) Play(SoundToPlay(&SFX_mnum, hundredths - 1));
        break;
      case SAY_WHOLE:
        // Thousands
        if (thousand > 0) {
          TensValue(thousand);
	  Play("thousand.wav");
        }
        // Hundred
        if (hundred > 0) {
	  Play(SoundToPlay(&SFX_mnum, hundred - 1));
	  Play("hundred.wav");
        }
        // Tens & Ones
        if (ones == 0) {
          if ((thousand + hundred) == 0) Play("mzero.wav");
        } else {
          TensValue(ones);
        }
        break;
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

  void SayAccept () { Play("maccept.wav"); }
  void SayAdjustBlackLevel() { Play("mblack.wav"); }
  void SayAdjustColorHue() { Play("mhue.wav"); }
  void SayAdjustWhiteLevel() { Play("mwhite.wav"); }
  void SayAltColor() { Play("malt.wav"); }
  void SayAuto() { Play("mauto.wav"); }
  void SayBaseColor() { Play("mbase.wav"); }
  void SayBatteryLevel() { Play("mbatt.wav"); }
  void SayTheBatteryLevelIs() { Play("battlevl.wav"); }
  void SayBlastColor() { Play("mblast.wav"); }
  void SayCancel() { Play("mcancel.wav"); }
  void SayChoreographyBegin() { Play("chreobgn.wav"); }
  void SayChoreographyEnd() { Play("chreoend.wav"); }
  void SayClashColor() { Play("mclash.wav"); }
  void SayClashDetectionLevel() { Play("mbmclash.wav"); }
  void SayClashLockupPosition() { Play("mlockpos.wav"); }
  void SayColorList() { Play("mcolorlt.wav"); }
  void SayColorMenu() { Play("mcolorsb.wav"); }
  void SayColorOptions() { Play("mcolorop.wav"); }
  void SayConfirm() { Play("mconfirm.wav"); }
  void SayConfirmSelection() { Play("maffirm.wav"); }
  void SayCoolDownOptions() { Play("mcooldwn.wav"); }
  void SayCopyColors() { Play("mcopyc.wav"); }
  void SayCopyPreset() { Play("mcopy.wav"); }
  void SayDeletePreset() { Play("mdelete.wav"); }
  void SayDisabled() { Play("mfalse.wav"); }
  void SayDown() { Play("mdown.wav"); } // Sound for decrease
  void SayDragColor() { Play("mdrag.wav"); }
  void SayDragSize() { Play("mdragsz.wav"); }
  void SayEditBladeLength() { Play("mlength.wav"); }
  void SayEditBrightness() { Play("mdim.wav"); }
  void SayEditClashThreshold() { Play("mclashth.wav"); }
  void SayEditColor() { Play("mcolor.wav"); }
  void SayEditControlSettings() { Play("mcontrol.wav"); }
  void SayEditFont() { Play("mfont.wav"); }
  void SayEditMode() { Play("medit.wav"); }   // 1-sec leading silence delay
  void SayEditSettings() { Play("msetting.wav"); }
  void SayEditStyle() { Play("mstyle.wav"); }
  void SayEditStyleSettings() { Play("mstylset.wav"); }
  void SayEditTrack() { Play("mtrack.wav"); }
  void SayEditVolume() { Play("mvolume.wav"); }
  void SayEmitterColor() { Play("memitter.wav"); }
  void SayEmitterSize() { Play("memitsz.wav"); }
  void SayEnabled() { Play("mtrue.wav"); }
  void SayEnterVolumeMenu() { Play("vmbegin.wav"); }
  void SayExit() { Play("mexit.wav"); }
  void SayForcePush() { Play("mpush.wav"); }
  void SayForcePushLength() { Play("mpushlen.wav"); }
  void SayGesturesOff() { Play("mgestoff.wav"); }
  void SayGesturesOn() { Play("mgeston.wav"); }
  void SayIgnitionColor() { Play("mignite.wav"); }
  void SayIgnitionDelay() { Play("migdelay.wav"); }
  void SayIgnitionOptions() { Play("migopt.wav"); }
  void SayIgnitionTime() { Play("mouttime.wav"); }
  void SayKeepRehearsal() { Play("rehrsold.wav"); }
  void SayLightningBlockColor() { Play("mlb.wav"); }
  void SayLockupColor() { Play("mlockup.wav"); }
  void SayLockupDelay() { Play("mlockdly.wav"); }
  void SayLoop() { Play("mloop.wav"); }
  void SayMainMenu() { Play("mmain.wav"); }
  void SayMaximum() { Play("mmax.wav"); }
  void SayMaximumClashStrength() { Play("maxclash.wav"); }
  void SayMaximumVolume() { Play("volmax.wav"); }
  void SayMeltSize() { Play("mmeltsz.wav"); }
  void SayMillis() { Play("mmillis.wav"); }
  void SayMinimum() { Play("mmin.wav"); }
  void SayMininumVolume() { Play("volmin.wav"); }
  void SayNoChoreographyAvailable() { Play("nochreo.wav"); }
  void SayOffColor() { Play("moff.wav"); }
  void SayOffOption() {Play("moffopt.wav"); }
  void SayOption() { Play("moptnum.wav"); }
  void SayPercent() { Play("mpercent.wav"); }
  void SayPostOffColor() { Play("mpstoff.wav"); }
  void SayPowerLock() { Play("mpwrlock.wav"); }
  void SayPowerUpOptions() { Play("mpwrup.wav"); }
  void SayPreonColor() { Play("mpreon.wav"); }
  void SayPreonOptions() { Play("mpreopt.wav"); }
  void SayPreonSize() { Play("mpreonsz.wav"); }
  void SayRandom() { Play("mrandom.wav"); }
  void SayRehearseBegin() { Play("rehrsbgn.wav"); }
  void SayRehearseEnd() { Play("rehrsend.wav"); }
  void SayRehearseNew() { Play("rehrsnew.wav"); } // rename?
  void SayResetColors() { Play("mresetc.wav"); }
  void SayRetractionColor() { Play("mretract.wav"); }
  void SayRetractionDelay() { Play("mrtdelay.wav"); }
  void SayRetractionOptions() { Play("mrtopt.wav"); }
  void SayRetractionTime() { Play("mintime.wav"); }
  void SayRevert() { Play("mrevert.wav"); }
  void SayRotate() { Play("mrotate.wav"); }
  void SaySave() { Play("msave.wav"); }
  void SaySelect() { Play("mselect.wav"); } // Sound for OK / Affirmative / Enter
  void SaySelectBlade() { Play("mblade.wav"); }
  void SaySelectEffect() { Play("meffect.wav"); }
  void SaySelectOption() { Play("moption.wav"); }
  void SaySelectPreset() { Play("mpreset.wav"); }
  void SaySelectStyle() { Play("mstylsel.wav"); }
  void SayStabIgnition() { Play("mstabon.wav"); }
  void SayStabColor() { Play("mstab.wav"); }
  void SayStyle() { Play("mstylnum.wav"); }
  void SayStyleMenu() { Play("mstylesb.wav"); }
  void SayStyleOptions() { Play("mstylopt.wav"); }
  void SaySwingColor() { Play("mswing.wav"); }
  void SaySwingIgnition() { Play("mswingon.wav"); }
  void SaySwingOnSpeed() { Play("mswingsp.wav"); }
  void SaySwingOption() { Play("mswngopt.wav"); }
  void SayThrustIgnition() { Play("mthrston.wav"); }
  void SayTwistIgnition() { Play("mtwiston.wav"); }
  void SayTwistRetraction() { Play("mtwstoff.wav"); }
  void SayUp() { Play("mup.wav"); } // Sound for increase
  void SayVolts() { Play("mvolts.wav"); }
  void SayVolumeMenuEnd() { Play("vmend.wav"); }
  void SayVolumeUp() { Play("volup.wav"); } // Sound for increasing volume
  void SayZoomingIn() { Play("mzoom.wav"); } // Sound for color menu "zooming in"
  void SayVolumeDown() { Play("voldown.wav"); } // Sound for decreasing volume
  void SayGestureMenu() { Play("mgestsub.wav"); }
  void SaySettingsMenu() { Play("msetsub.wav"); }
  void SayStyleSettings() { Play("stylstm.wav"); }

#ifdef SAY_COLOR_LIST
  void SayColor(ColorNumber n) {
    Play(SoundToPlay(&SFX_clrlst, n - 1));
  }
#endif

  void SayBool(bool v) {
    if (v) {
      SayEnabled();
    } else {
      SayDisabled();
    }
  }
};

SoundLibrary sound_library_;

#endif  // SOUND_SOUND_LIBRARY_H
