#ifndef PROPS_PROP_BASE_H
#define PROPS_PROP_BASE_H

class SaveGlobalStateFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(volume, -1);
  }
  int volume;
};

class SavePresetStateFile : public ConfigFile {
public:
  void SetVariable(const char* variable, float v) override {
    CONFIG_VARIABLE(preset, 0);
  }
  int preset;
};


// Base class for props.
class PropBase : CommandParser, Looper, protected SaberBase {
public:
  PropBase() : CommandParser() {}
  BladeStyle* current_style(){
#if NUM_BLADES == 0
    return nullptr;
#else    
    return current_config->blade1->current_style();
#endif    
  }

  bool NeedsPower() {
    if (SaberBase::IsOn()) return true;
    if (current_style() && current_style()->NoOnOff())
      return true;
    return false;
  }

  int32_t muted_volume_ = 0;
  bool SetMute(bool muted) {
#ifdef ENABLE_AUDIO
    if (muted) {
      if (dynamic_mixer.get_volume()) {
        muted_volume_ = dynamic_mixer.get_volume();
        dynamic_mixer.set_volume(0);
        return true;
      }
    } else {
      if (muted_volume_) {
        dynamic_mixer.set_volume(muted_volume_);
        muted_volume_ = 0;
        return true;
      }
    }
#endif
    return false;
  }

  bool unmute_on_deactivation_ = false;
  uint32_t activated_ = 0;
  uint32_t last_clash_ = 0;
  uint32_t clash_timeout_ = 100;
  bool clash_pending_ = false;
  bool pending_clash_is_stab_ = false;

  bool on_pending_ = false;
  uint32_t on_pending_base_;
  uint32_t on_pending_delay_;

  virtual bool IsOn() {
    return SaberBase::IsOn() || on_pending_;
  }

  virtual void On() {
    if (IsOn()) return;
    if (current_style() && current_style()->NoOnOff())
      return;
    activated_ = millis();
    STDOUT.println("Ignition.");
    MountSDCard();
    EnableAmplifier();
    SaberBase::RequestMotion();

    // Avoid clashes a little bit while turning on.
    // It might be a "clicky" power button...
    IgnoreClash(300);

    float preon_time = 0.0;
    SaberBase::DoPreOn(&preon_time);
    if (preon_time > 0.0) {
      on_pending_ = true;
      on_pending_base_ = millis();
      on_pending_delay_ = preon_time * 1000;
    } else {
      SaberBase::TurnOn();
    }
  }

  virtual void Off(OffType off_type = OFF_NORMAL) {
    if (on_pending_) {
      // Or is it better to wait until we turn on, and then turn off?
      on_pending_ = false;
      return;
    }
    if (!SaberBase::IsOn()) return;
    if (SaberBase::Lockup()) {
      SaberBase::DoEndLockup();
      SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
    }
#ifndef DISABLE_COLOR_CHANGE
    if (SaberBase::GetColorChangeMode() != SaberBase::COLOR_CHANGE_MODE_NONE) {
      ToggleColorChangeMode();
    }
#endif    
    SaberBase::TurnOff(off_type);
    if (unmute_on_deactivation_) {
      unmute_on_deactivation_ = false;
#ifdef ENABLE_AUDIO
      // We may also need to stop any thing else that generates noise..
      for (size_t i = 0; i < NELEM(wav_players); i++) {
        wav_players[i].Stop();
      }
#endif
      SetMute(false);
    }
  }

  void IgnoreClash(size_t ms) {
    if (clash_pending_) return;
    uint32_t now = millis();
    uint32_t time_since_last_clash = now - last_clash_;
    if (time_since_last_clash < clash_timeout_) {
      ms = std::max<size_t>(ms, clash_timeout_ - time_since_last_clash);
    }
    last_clash_ = now;
    clash_timeout_ = ms;
  }

  virtual void Clash2(bool stab) {
    if (Event(BUTTON_NONE, stab ? EVENT_STAB : EVENT_CLASH)) {
      IgnoreClash(400);
    } else {
      IgnoreClash(100);
      if (SaberBase::IsOn()) {
        if (stab) {
          SaberBase::DoStab();
        } else {
          SaberBase::DoClash();
        }
      }
    }
  }

  virtual void Clash(bool stab) {
    // No clashes in lockup mode.
    if (SaberBase::Lockup()) return;
    // TODO: Pick clash randomly and/or based on strength of clash.
    uint32_t t = millis();
    if (t - last_clash_ < clash_timeout_) {
      last_clash_ = t; // Vibration cancellation
      return;
    }
    if (current_modifiers & ~MODE_ON) {
      // Some button is pressed, that means that we need to delay the clash a little
      // to see if was caused by a button *release*.
      last_clash_ = millis();
      clash_timeout_ = 3;
      clash_pending_ = true;
      pending_clash_is_stab_ = stab;
      return;
    }
    Clash2(stab);
  }

  bool chdir(const char* dir) {
    if (strlen(dir) > 1 && dir[strlen(dir)-1] == '/') {
      STDOUT.println("Directory must not end with slash.");
      return false;
    }
#ifdef ENABLE_AUDIO
    smooth_swing_v2.Deactivate();
    looped_swing_wrapper.Deactivate();
    hybrid_font.Deactivate();

    // Stop all sound!
    // TODO: Move scanning to wav-playing interrupt level so we can
    // interleave things without worry about memory corruption.
    for (size_t i = 0; i < NELEM(wav_players); i++) {
      wav_players[i].Stop();
    }
#endif

    char *b = current_directory;
    for (const char *a = dir; *a; a++) {
      // Skip trailing slash
      if (*a == '/' && (a[1] == 0 || a[1] == ';'))
        continue;
      if (*a == ';') {
        *(b++) = 0;
        continue;
      }
      *(b++) = *a;
    }
    // Two zeroes at end!
    *(b++) = 0;
    *(b++) = 0;

#ifdef ENABLE_AUDIO
    Effect::ScanCurrentDirectory();
    SaberBase* font = NULL;
    hybrid_font.Activate();
    font = &hybrid_font;
    if (font) {
      smooth_swing_config.ReadInCurrentDir("smoothsw.ini");
      if (SFX_lswing) {
        smooth_swing_cfx_config.ReadInCurrentDir("font_config.txt");
	// map CFX values to Proffie (sourced from font_config.txt in font folder)
	smooth_swing_config.SwingSensitivity = smooth_swing_cfx_config.smooth_sens;
	smooth_swing_config.MaximumHumDucking = smooth_swing_cfx_config.smooth_dampen;
	smooth_swing_config.SwingSharpness = smooth_swing_cfx_config.smooth_sharp;
	smooth_swing_config.SwingStrengthThreshold = smooth_swing_cfx_config.smooth_gate;
	smooth_swing_config.Transition1Degrees = smooth_swing_cfx_config.smooth_width1;
	smooth_swing_config.Transition2Degrees = smooth_swing_cfx_config.smooth_width2;
	smooth_swing_config.MaxSwingVolume = smooth_swing_cfx_config.smooth_gain * 3 / 100;
	smooth_swing_config.AccentSwingSpeedThreshold = smooth_swing_cfx_config.hswing;
	smooth_swing_config.Version = 2;
      } else if (!SFX_swingl) {
	smooth_swing_config.Version = 0;
      }
      switch (smooth_swing_config.Version) {
        case 1:
          looped_swing_wrapper.Activate(font);
          break;
        case 2:
          smooth_swing_v2.Activate(font);
          break;
      }
    }
//    EnableBooster();
#endif
    return false;
  }

  void SaveVolumeIfNeeded() {
#ifdef SAVE_VOLUME
    if (dynamic_mixer.get_volume() != saved_global_state.volume) {
      SaveGlobalState();
    }
#endif
  }

  void SaveColorChangeIfNeeded() {
#ifdef SAVE_COLOR_CHANGE
    if (current_preset_.variation != SaberBase::GetCurrentVariation()) {
      current_preset_.variation = SaberBase::GetCurrentVariation();
      current_preset_.Save();
    }
#endif
  }

  void PollSaveColorChange() {
#ifdef ENABLE_AUDIO
    if (AmplifierIsActive()) return; // Do it later
#endif
    SaveColorChangeIfNeeded();
    SaveVolumeIfNeeded();
  }

  // Select preset (font/style)
  virtual void SetPreset(int preset_num, bool announce) {
    TRACE("start");
#ifdef IDLE_OFF_TIME
    last_on_time_ = millis();
#endif
    bool on = SaberBase::IsOn();
    if (on) Off();
    SaveColorChangeIfNeeded();
    // First free all styles, then allocate new ones to avoid memory
    // fragmentation.
#define UNSET_BLADE_STYLE(N) \
    delete current_config->blade##N->UnSetStyle();
    ONCEPERBLADE(UNSET_BLADE_STYLE)
    current_preset_.SetPreset(preset_num);
    if (announce) {
      if (current_preset_.name.get()) {
        SaberBase::DoMessage(current_preset_.name.get());
      } else {
        char message[64];
        strcpy(message, "Preset: ");
        itoa(current_preset_.preset_num + 1,
             message + strlen(message), 10);
        strcat(message, "\n");
        strncat(message + strlen(message),
                current_preset_.font.get(), sizeof(message) - strlen(message));
        message[sizeof(message) - 1] = 0;
        SaberBase::DoMessage(message);
      }
    }

#define SET_BLADE_STYLE(N) \
    current_config->blade##N->SetStyle(style_parser.Parse(current_preset_.current_style##N.get()));
    ONCEPERBLADE(SET_BLADE_STYLE)
    chdir(current_preset_.font.get());

#ifdef SAVE_COLOR_CHANGE
    SaberBase::SetVariation(current_preset_.variation);
#else
    SaberBase::SetVariation(0);
#endif

    if (on) On();
    if (announce) SaberBase::DoNewFont();
    TRACE("end");
  }

  // Go to the next Preset.
  virtual void next_preset() {
#ifdef SAVE_PRESET
    SaveState(current_preset_.preset_num + 1);
#endif
    SetPreset(current_preset_.preset_num + 1, true);
  }

  // Go to the previous Preset.
  virtual void previous_preset() {
#ifdef SAVE_PRESET
    SaveState(current_preset_.preset_num - 1);
#endif
    SetPreset(current_preset_.preset_num - 1, true);
  }

  // Rotates presets backwards and saves.
  virtual void rotate_presets() {
#ifdef IDLE_OFF_TIME
    last_on_time_ = millis();
#endif
#ifdef ENABLE_AUDIO
    beeper.Beep(0.05, 2000.0);
#endif
    LOCK_SD(true);
    current_preset_.Load(-1);  // load last preset
    current_preset_.SaveAt(0); // save in first position, shifting all other presets down
    LOCK_SD(false);
    SetPreset(0, true);
  }

#ifdef BLADE_DETECT_PIN
  bool blade_detected_ = false;
#endif

  // Measure and return the blade identifier resistor.
  float id() {
    BLADE_ID_CLASS_INTERNAL blade_id;
    float ret = blade_id.id();
    STDOUT << "ID: " << ret << "\n";
#ifdef SPEAK_BLADE_ID
    talkie.Say(spI);
    talkie.Say(spD);
    talkie.SayNumber((int)ret);
#endif    
#ifdef BLADE_DETECT_PIN
    if (!blade_detected_) {
      STDOUT << "NO ";
      ret += NO_BLADE;
    }
    STDOUT << "Blade Detected\n";
#endif
    return ret;
  }

  // Called from setup to identify the blade and select the right
  // Blade driver, style and sound font.
  void FindBlade() {
    size_t best_config = 0;
    if (NELEM(blades) > 1) {
      float resistor = id();

      float best_err = 100000000.0;
      for (size_t i = 0; i < NELEM(blades); i++) {
        float err = fabsf(resistor - blades[i].ohm);
        if (err < best_err) {
          best_config = i;
          best_err = err;
        }
      }
    }
    STDOUT.print("blade= ");
    STDOUT.println(best_config);
    current_config = blades + best_config;

#define ACTIVATE(N) do {     \
    if (!current_config->blade##N) goto bad_blade;  \
    current_config->blade##N->Activate();           \
  } while(0);

    ONCEPERBLADE(ACTIVATE);
    RestoreGlobalState();
#ifdef SAVE_PRESET
    ResumePreset();
#else
    SetPreset(0, false);
#endif
    return;

#if NUM_BLADES != 0    

  bad_blade:
    STDOUT.println("BAD BLADE");
#ifdef ENABLE_AUDIO
    talkie.Say(talkie_error_in_15, 15);
    talkie.Say(talkie_blade_array_15, 15);
#endif

#endif    
  }

  void ResumePreset() {
    SavePresetStateFile savestate;
    savestate.ReadINIFromSaveDir("curstate");
    SetPreset(savestate.preset, false);
  }

  void WriteState(const char *filename, int preset) {
    PathHelper fn(GetSaveDir(), filename);
    LOCK_SD(true);
    FileReader out;
    LSFS::Remove(fn);
    out.Create(fn);
    out.write_key_value("installed", install_time);
    out.write_key_value("preset", preset);
    out.write_key_value("end", "1");
    out.Close();
    LOCK_SD(false);
  }

  void SaveState(int preset) {
    STDOUT.println("Saving Current Preset");
    WriteState("curstate.tmp", preset);
    WriteState("curstate.ini", preset);
  }

  SaveGlobalStateFile saved_global_state;
  void RestoreGlobalState() {
#ifdef SAVE_VOLUME
    saved_global_state.ReadINIFromDir(NULL, "global");
    if (saved_global_state.volume >= 0) {
      dynamic_mixer.set_volume(clampi32(saved_global_state.volume, 0, VOLUME));
    }
#endif    
  }

  void WriteGlobalState(const char* filename) {
    LOCK_SD(true);
    FileReader out;
    LSFS::Remove(filename);
    out.Create(filename);
    out.write_key_value("installed", install_time);
#ifdef ENABLE_AUDIO    
    out.write_key_value("volume", muted_volume_ ? muted_volume_ : dynamic_mixer.get_volume());
#endif    
    out.write_key_value("end", "1");
    out.Close();
    LOCK_SD(false);
  }

  void SaveGlobalState() {
#ifdef SAVE_VOLUME
    STDOUT.println("Saving Global State");
    WriteGlobalState("global.tmp");
    WriteGlobalState("global.ini");
    saved_global_state.volume = dynamic_mixer.get_volume();
#endif    
  }
  
  void FindBladeAgain() {
    if (!current_config) {
      // FindBlade() hasn't been called yet - ignore this.
      return;
    }
    // Reverse everything that FindBlade does.

    // First free all styles, then allocate new ones to avoid memory
    // fragmentation.
    ONCEPERBLADE(UNSET_BLADE_STYLE)

#define DEACTIVATE(N) do {                      \
    if (current_config->blade##N)               \
      current_config->blade##N->Deactivate();   \
  } while(0);

    ONCEPERBLADE(DEACTIVATE);

    FindBlade();
  }

  void SB_Message(const char* text) override {
    STDOUT.print("DISPLAY: ");
    STDOUT.println(text);
  }

  float peak = 0.0;
  Vec3 at_peak;
  virtual void DoAccel(const Vec3& accel, bool clear) {
    SaberBase::DoAccel(accel, clear);
    accel_loop_counter_.Update();
    if (clear) accel_ = accel;
    Vec3 diff = (accel - fusor.down());
    if (clear) diff = Vec3(0,0,0);
    float v = diff.len();
    // If we're spinning the saber, require a stronger acceleration
    // to activate the clash.
    if (v > CLASH_THRESHOLD_G + fusor.gyro().len() / 200.0) {
      if ( (accel_ - fusor.down()).len2() > (accel - fusor.down()).len2() ) {
        diff = -diff;
      }
      bool stab = diff.x < - 2.0 * sqrtf(diff.y * diff.y + diff.z * diff.z) &&
#if 0
      Vec3 speed = fusor.speed();
        // Speed checks simply don't work yet
        speed.y * speed.y + speed.z * speed.z < 5.0 && // TODO: Make this tighter
        speed.x > 0.1 &&
#endif
        fusor.swing_speed() < 150;

#if 1
      STDOUT << "ACCEL: " << accel
             << " diff=" << diff
             << " v=" << v
             << " fgl=" << (fusor.gyro().len() / 200.0)
             << " accel_=" << accel_
             << " clear=" << clear
             << " millis=" << millis()
             << " swing_speed=" << fusor.swing_speed()
             << " mss=" << fusor.mss()
             << " stab=" << stab
             << "\n";
#endif
      // Needs de-bouncing
      Clash(stab);
    }
    if (v > peak) {
      peak = v;
      at_peak = accel_ - accel;
    }
    accel_ = accel;
    if (monitor.ShouldPrint(Monitoring::MonitorClash)) {
      STDOUT.print("ACCEL: ");
      STDOUT.print(accel.x);
      STDOUT.print(", ");
      STDOUT.print(accel.y);
      STDOUT.print(", ");
      STDOUT.print(accel.z);
      STDOUT.print(" peak ");
      STDOUT.print(at_peak.x);
      STDOUT.print(", ");
      STDOUT.print(at_peak.y);
      STDOUT.print(", ");
      STDOUT.print(at_peak.z);
      STDOUT.print(" (");
      STDOUT.print(peak);
      STDOUT.println(")");
      peak = 0.0;
    }
  }

  virtual void DoMotion(const Vec3& motion, bool clear) {
    SaberBase::DoMotion(motion, clear);
  }

  void SB_Top(uint64_t total_cycles) override {
    STDOUT.print("Acceleration measurements per second: ");
    accel_loop_counter_.Print();
    STDOUT.println("");
  }

  enum StrokeType {
    TWIST_CLOSE,
    TWIST_LEFT,
    TWIST_RIGHT,

    SHAKE_CLOSE,
    SHAKE_FWD,
    SHAKE_REW
  };
  struct Stroke {
    StrokeType type;
    uint32_t start_millis;
    uint32_t end_millis;
    uint32_t length() const { return end_millis - start_millis; }
  };

  Stroke strokes[5];

  void MonitorStrokes() {
    if (monitor.IsMonitoring(Monitoring::MonitorStrokes)) {
      STDOUT.print("Stroke: ");
      switch (strokes[NELEM(strokes)-1].type) {
        case TWIST_LEFT:
          STDOUT.print("TwistLeft");
          break;
        case TWIST_RIGHT:
          STDOUT.print("TwistRight");
          break;
        case SHAKE_FWD:
          STDOUT.print("Thrust");
          break;
        case SHAKE_REW:
          STDOUT.print("Yank");
          break;
        default: break;
      }
      STDOUT << " len = " << strokes[NELEM(strokes)-1].length();
      uint32_t separation =
        strokes[NELEM(strokes)-1].start_millis -
        strokes[NELEM(strokes)-2].end_millis;
      STDOUT << " separation=" << separation
             << " mss=" << fusor.mss()
             << " swspd=" << fusor.swing_speed()
             << "\n";
    }
  }

  StrokeType GetStrokeGroup(StrokeType a) {
    switch (a) {
      case TWIST_CLOSE:
      case TWIST_LEFT:
      case TWIST_RIGHT:
        return TWIST_CLOSE;
      case SHAKE_CLOSE:
      case SHAKE_FWD:
      case SHAKE_REW:
        break;
    }
    return SHAKE_CLOSE;
  }

  bool ShouldClose(StrokeType a, StrokeType b) {
    // Don't close if it's the same exact stroke
    if (a == b) return false;
    // Different stroke in same stroke group -> close
    if (GetStrokeGroup(a) == GetStrokeGroup(b)) return true;
    // New stroke in different group -> close
    if (GetStrokeGroup(b) != b) return true;
    return false;
  }

  bool DoGesture(StrokeType gesture) {
    if (gesture == strokes[NELEM(strokes)-1].type) {
      if (strokes[NELEM(strokes)-1].end_millis == 0) {
        // Stroke not done, wait.
        return false;
      }
      if (millis() - strokes[NELEM(strokes)-1].end_millis < 50)  {
        // Stroke continues
        strokes[NELEM(strokes)-1].end_millis = millis();
        return false;
      }
    }
    if (strokes[NELEM(strokes) - 1].end_millis == 0 &&
        GetStrokeGroup(gesture) == GetStrokeGroup(strokes[NELEM(strokes) - 1].type)) {
      strokes[NELEM(strokes) - 1].end_millis = millis();
      MonitorStrokes();
      return true;
    }
    // Exit here if it's a *_CLOSE stroke.
    if (GetStrokeGroup(gesture) == gesture) return false;
    // If last stroke is very short, just write over it.
    if (strokes[NELEM(strokes)-1].end_millis -
        strokes[NELEM(strokes)-1].start_millis > 10) {
      for (size_t i = 0; i < NELEM(strokes) - 1; i++) {
        strokes[i] = strokes[i+1];
      }
    }
    strokes[NELEM(strokes)-1].type = gesture;
    strokes[NELEM(strokes)-1].start_millis = millis();
    strokes[NELEM(strokes)-1].end_millis = 0;
    return false;
  }

  // The prop should call this from Loop() if it wants to detect twists.
  void DetectTwist() {
    Vec3 gyro = fusor.gyro();
    bool process = false;
    if (fabsf(gyro.x) > 200.0 &&
        fabsf(gyro.x) > 3.0f * abs(gyro.y) &&
        fabsf(gyro.x) > 3.0f * abs(gyro.z)) {
      process = DoGesture(gyro.x > 0 ? TWIST_LEFT : TWIST_RIGHT);
    } else {
      process = DoGesture(TWIST_CLOSE);
    }
    if (process) {
      if ((strokes[NELEM(strokes)-1].type == TWIST_LEFT &&
	   strokes[NELEM(strokes)-2].type == TWIST_RIGHT) ||
	  (strokes[NELEM(strokes)-1].type == TWIST_RIGHT &&
	   strokes[NELEM(strokes)-2].type == TWIST_LEFT)) {
	if (strokes[NELEM(strokes) -1].length() > 90UL &&
	    strokes[NELEM(strokes) -1].length() < 300UL &&
	    strokes[NELEM(strokes) -2].length() > 90UL &&
	    strokes[NELEM(strokes) -2].length() < 300UL) {
	  uint32_t separation =
	    strokes[NELEM(strokes)-1].start_millis -
	    strokes[NELEM(strokes)-2].end_millis;
	  if (separation < 200UL) {
	    STDOUT.println("TWIST");
	    // We have a twisting gesture.
	    Event(BUTTON_NONE, EVENT_TWIST);
	  }
	}
      }
    }
  }

  // The prop should call this from Loop() if it wants to detect shakes.
  void DetectShake() {
    Vec3 mss = fusor.mss();
    bool process = false;
    if (mss.y * mss.y + mss.z * mss.z < 16.0 &&
        (mss.x > 7 || mss.x < -6)  &&
        fusor.swing_speed() < 150) {
      process = DoGesture(mss.x > 0 ? SHAKE_FWD : SHAKE_REW);
    } else {
      process = DoGesture(SHAKE_CLOSE);
    }
    if (process) {
      int i;
      for (i = 0; i < 5; i++) {
	if (strokes[NELEM(strokes)-1-i].type !=
	    ((i & 1) ? SHAKE_REW : SHAKE_FWD)) break;
	if (i) {
	  uint32_t separation =
	    strokes[NELEM(strokes)-i].start_millis -
	    strokes[NELEM(strokes)-1-i].end_millis;
	  if (separation > 250) break;
	}
      }
      if (i == 5) {
	strokes[NELEM(strokes)-1].type = SHAKE_CLOSE;
	Event(BUTTON_NONE, EVENT_SHAKE);
      }
    }
  }

  bool swinging_ = false;
  // The prop should call this from Loop() if it wants to detect swings as an event.
  void DetectSwing() {
    if (!swinging_ && fusor.swing_speed() > 250) {
      swinging_ = true;
      Event(BUTTON_NONE, EVENT_SWING);
    }
    if (swinging_ && fusor.swing_speed() < 100) {
      swinging_ = false;
    }
  }
  
  void SB_Motion(const Vec3& gyro, bool clear) override {
    if (monitor.ShouldPrint(Monitoring::MonitorGyro)) {
      // Got gyro data
      STDOUT.print("GYRO: ");
      STDOUT.print(gyro.x);
      STDOUT.print(", ");
      STDOUT.print(gyro.y);
      STDOUT.print(", ");
      STDOUT.println(gyro.z);
    }
  }

  Vec3 accel_;

  void StartOrStopTrack() {
#ifdef ENABLE_AUDIO
    if (track_player_) {
      track_player_->Stop();
      track_player_.Free();
    } else {
      MountSDCard();
      EnableAmplifier();
      track_player_ = GetFreeWavPlayer();
      if (track_player_) {
	track_player_->Play(current_preset_.track.get());
      } else {
        STDOUT.println("No available WAV players.");
      }
    }
#else
    STDOUT.println("Audio disabled.");
#endif
  }

  void ListTracks(const char* dir) {
    if (!LSFS::Exists(dir)) return;
    for (LSFS::Iterator i2(dir); i2; ++i2) {
      if (endswith(".wav", i2.name()) && i2.size() > 200000) {
	STDOUT << dir << "/" << i2.name() << "\n";
      }
    }
  }

  virtual void LowBatteryOff() {
    if (SaberBase::IsOn()) {
      STDOUT.print("Battery low, turning off. Battery voltage: ");
      STDOUT.println(battery_monitor.battery());
      Off();
    }
  }

  virtual void CheckLowBattery() {
    if (battery_monitor.low()) {
      if (current_style() && !current_style()->Charging()) {
        LowBatteryOff();
        if (millis() - last_beep_ > 15000) {  // (was 5000)
          STDOUT << "Low battery: " << battery_monitor.battery() << " volts\n";
          SaberBase::DoLowBatt();
          last_beep_ = millis();
	}
      }
    }
  }

  uint32_t last_beep_;
  float current_tick_angle_ = 0.0;

  void Loop() override {
    if (on_pending_ && millis() - on_pending_base_ >= on_pending_delay_) {
      on_pending_ = false;
      SaberBase::TurnOn();
    }

    if (clash_pending_ && millis() - last_clash_ >= clash_timeout_) {
      clash_pending_ = false;
      Clash2(pending_clash_is_stab_);
    }
    CheckLowBattery();
#ifdef ENABLE_AUDIO
    if (track_player_ && !track_player_->isPlaying()) {
      track_player_.Free();
    }
#endif

#ifndef DISABLE_COLOR_CHANGE
#define TICK_ANGLE (M_PI * 2 / 12)
    switch (SaberBase::GetColorChangeMode()) {
      case SaberBase::COLOR_CHANGE_MODE_NONE:
        break;
      case SaberBase::COLOR_CHANGE_MODE_STEPPED: {
        float a = fusor.angle2() - current_tick_angle_;
        if (a > M_PI) a-=M_PI*2;
        if (a < -M_PI) a+=M_PI*2;
        if (a > TICK_ANGLE * 2/3) {
          current_tick_angle_ += TICK_ANGLE;
          if (current_tick_angle_ > M_PI) current_tick_angle_ -= M_PI * 2;
          STDOUT << "TICK+\n";
          SaberBase::UpdateVariation(1);
        }
        if (a < -TICK_ANGLE * 2/3) {
          current_tick_angle_ -= TICK_ANGLE;
          if (current_tick_angle_ < M_PI) current_tick_angle_ += M_PI * 2;
          STDOUT << "TICK-\n";
          SaberBase::UpdateVariation(-1);
        }
        break;
      }
      case SaberBase::COLOR_CHANGE_MODE_SMOOTH:
        float a = fmodf(fusor.angle2() - current_tick_angle_, M_PI * 2);
        SaberBase::SetVariation(0x7fff & (int32_t)(a * (32768 / (M_PI * 2))));
        break;
    }
    if (monitor.ShouldPrint(Monitoring::MonitorVariation)) {
      STDOUT << " variation = " << SaberBase::GetCurrentVariation()
             << " ccmode = " << SaberBase::GetColorChangeMode()
//	     << " color = " << current_config->blade1->current_style()->getColor(0)
             << "\n";
    }

#endif


#ifdef IDLE_OFF_TIME
    if (SaberBase::IsOn() ||
        (current_style() && current_style()->Charging())) {
      last_on_time_ = millis();
    }
    if (millis() - last_on_time_ > IDLE_OFF_TIME) {
      SaberBase::DoOff(OFF_IDLE);
      last_on_time_ = millis();
    }
#endif

    PollSaveColorChange();
  }

#ifdef IDLE_OFF_TIME
  uint32_t last_on_time_;
#endif

#ifndef DISABLE_COLOR_CHANGE
  void ToggleColorChangeMode() {
    if (!current_style()) return;
    if (SaberBase::GetColorChangeMode() == SaberBase::COLOR_CHANGE_MODE_NONE) {
      current_tick_angle_ = fusor.angle2();
      bool handles_color_change = false;
#define CHECK_SUPPORTS_COLOR_CHANGE(N) \
      handles_color_change |= current_config->blade##N->current_style() && current_config->blade##N->current_style()->IsHandled(HANDLED_FEATURE_CHANGE_TICKED);
      ONCEPERBLADE(CHECK_SUPPORTS_COLOR_CHANGE)
      if (!handles_color_change) {
        STDOUT << "Entering smooth color change mode.\n";
        current_tick_angle_ -= SaberBase::GetCurrentVariation() * M_PI * 2 / 32768;
        current_tick_angle_ = fmodf(current_tick_angle_, M_PI * 2);

        SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_SMOOTH);
      } else {
#ifdef COLOR_CHANGE_DIRECT
        STDOUT << "Color change, TICK+\n";
        SaberBase::UpdateVariation(1);
#else
        STDOUT << "Entering stepped color change mode.\n";
        SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_STEPPED);
#endif
      }
    } else {
      STDOUT << "Color change mode done, variation = " << SaberBase::GetCurrentVariation() << "\n";
      SaberBase::SetColorChangeMode(SaberBase::COLOR_CHANGE_MODE_NONE);
    }
  }
#endif // DISABLE_COLOR_CHANGE  

  void PrintButton(uint32_t b) {
    if (b & BUTTON_POWER) STDOUT.print("Power");
    if (b & BUTTON_AUX) STDOUT.print("Aux");
    if (b & BUTTON_AUX2) STDOUT.print("Aux2");
    if (b & BUTTON_UP) STDOUT.print("Up");
    if (b & BUTTON_DOWN) STDOUT.print("Down");
    if (b & BUTTON_LEFT) STDOUT.print("Left");
    if (b & BUTTON_RIGHT) STDOUT.print("Right");
    if (b & BUTTON_SELECT) STDOUT.print("Select");
    if (b & BUTTON_BLADE_DETECT) STDOUT.print("Select");
    if (b & MODE_ON) STDOUT.print("On");
  }

  void PrintEvent(uint32_t e) {
    int cnt = 0;
    if (e >= EVENT_FIRST_PRESSED &&
	e <= EVENT_FOURTH_CLICK_LONG) {
      cnt = (e - EVENT_PRESSED) / (EVENT_SECOND_PRESSED - EVENT_FIRST_PRESSED);
      e -= (EVENT_SECOND_PRESSED - EVENT_FIRST_PRESSED) * cnt;
    }
    switch (e) {
      case EVENT_NONE: STDOUT.print("None"); break;
      case EVENT_PRESSED: STDOUT.print("Pressed"); break;
      case EVENT_RELEASED: STDOUT.print("Released"); break;
      case EVENT_HELD: STDOUT.print("Held"); break;
      case EVENT_HELD_MEDIUM: STDOUT.print("HeldMedium"); break;
      case EVENT_HELD_LONG: STDOUT.print("HeldLong"); break;
      case EVENT_CLICK_SHORT: STDOUT.print("Shortclick"); break;
      case EVENT_CLICK_LONG: STDOUT.print("Longclick"); break;
      case EVENT_SAVED_CLICK_SHORT: STDOUT.print("SavedShortclick"); break;
      case EVENT_LATCH_ON: STDOUT.print("On"); break;
      case EVENT_LATCH_OFF: STDOUT.print("Off"); break;
      case EVENT_STAB: STDOUT.print("Stab"); break;
      case EVENT_SWING: STDOUT.print("Swing"); break;
      case EVENT_SHAKE: STDOUT.print("Shake"); break;
      case EVENT_TWIST: STDOUT.print("Twist"); break;
      case EVENT_CLASH: STDOUT.print("Clash"); break;
      case EVENT_THRUST: STDOUT.print("Thrust"); break;
      case EVENT_PUSH: STDOUT.print("Push"); break;
      default: STDOUT.print("?"); STDOUT.print(e); break;
    }
    if (cnt) {
      STDOUT.print('#');
      STDOUT.print(cnt);
    }
  }

  void PrintEvent(enum BUTTON button, EVENT event) {
    STDOUT.print("EVENT: ");
    if (button) {
      PrintButton(button);
      STDOUT.print("-");
    }
    PrintEvent(event);
    if (current_modifiers & ~button) {
      STDOUT.print(" mods ");
      PrintButton(current_modifiers);
    }
    if (IsOn()) STDOUT.print(" ON");
    STDOUT.print(" millis=");
    STDOUT.println(millis());
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "id")) {
      id();
      return true;
    }
    if (!strcmp(cmd, "scanid")) {
      FindBladeAgain();
      return true;
    }
    if (!strcmp(cmd, "on")) {
      On();
      return true;
    }
    if (!strcmp(cmd, "off")) {
      Off();
      return true;
    }
    if (!strcmp(cmd, "get_on")) {
      STDOUT.println(IsOn());
      return true;
    }
    if (!strcmp(cmd, "clash")) {
      Clash(false);
      return true;
    }
    if (!strcmp(cmd, "stab")) {
      Clash(true);
      return true;
    }
    if (!strcmp(cmd, "force")) {
      SaberBase::DoForce();
      return true;
    }
    if (!strcmp(cmd, "blast")) {
      // Avoid the base and the very tip.
      // TODO: Make blast only appear on one blade!
      SaberBase::DoBlast();
      return true;
    }
    if (!strcmp(cmd, "lock") || !strcmp(cmd, "lockup")) {
      STDOUT.print("Lockup ");
      if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
        SaberBase::SetLockup(SaberBase::LOCKUP_NORMAL);
        SaberBase::DoBeginLockup();
        STDOUT.println("ON");
      } else {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        STDOUT.println("OFF");
      }
      return true;
    }
    if (!strcmp(cmd, "drag")) {
      STDOUT.print("Drag ");
      if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
        SaberBase::SetLockup(SaberBase::LOCKUP_DRAG);
        SaberBase::DoBeginLockup();
        STDOUT.println("ON");
      } else {
        SaberBase::DoEndLockup();
        SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
        STDOUT.println("OFF");
      }
      return true;
    }
    if (!strcmp(cmd, "lblock") || !strcmp(cmd, "lb")) {
      STDOUT.print("lblock ");
      if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
	SaberBase::SetLockup(SaberBase::LOCKUP_LIGHTNING_BLOCK);
	SaberBase::DoBeginLockup();
	STDOUT.println("ON");
      } else {
	SaberBase::DoEndLockup();
	SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
	STDOUT.println("OFF");
      }
      return true;
    }
    if (!strcmp(cmd, "melt")) {
      STDOUT.print("melt ");
      if (SaberBase::Lockup() == SaberBase::LOCKUP_NONE) {
	SaberBase::SetLockup(SaberBase::LOCKUP_MELT);
	SaberBase::DoBeginLockup();
	STDOUT.println("ON");
      } else {
	SaberBase::DoEndLockup();
	SaberBase::SetLockup(SaberBase::LOCKUP_NONE);
	STDOUT.println("OFF");
      }
      return true;
    }

#ifdef ENABLE_AUDIO

#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "beep")) {
      beeper.Beep(0.5, 293.66 * 2);
      beeper.Beep(0.5, 329.33 * 2);
      beeper.Beep(0.5, 261.63 * 2);
      beeper.Beep(0.5, 130.81 * 2);
      beeper.Beep(1.0, 196.00 * 2);
      return true;
    }
#endif
    if (!strcmp(cmd, "play")) {
      if (!arg) {
        StartOrStopTrack();
        return true;
      }
      MountSDCard();
      EnableAmplifier();
      RefPtr<BufferedWavPlayer> player = GetFreeWavPlayer();
      if (player) {
        STDOUT.print("Playing ");
        STDOUT.println(arg);
	if (!player->PlayInCurrentDir(arg))
	  player->Play(arg);
      } else {
        STDOUT.println("No available WAV players.");
      }
      return true;
    }
    if (!strcmp(cmd, "play_track")) {
      if (!arg) {
        StartOrStopTrack();
        return true;
      }
      if (track_player_) {
        track_player_->Stop();
        track_player_.Free();
      }
      MountSDCard();
      EnableAmplifier();
      track_player_ = GetFreeWavPlayer();
      if (track_player_) {
        STDOUT.print("Playing ");
        STDOUT.println(arg);
	if (!track_player_->PlayInCurrentDir(arg))
	  track_player_->Play(arg);
      } else {
        STDOUT.println("No available WAV players.");
      }
      return true;
    }
    if (!strcmp(cmd, "stop_track")) {
      if (track_player_) {
        track_player_->Stop();
        track_player_.Free();
      }
      return true;
    }
    if (!strcmp(cmd, "get_track")) {
      if (track_player_) {
        STDOUT.println(track_player_->Filename());
      }
      return true;
    }
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "volumes")) {
      for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
        STDOUT.print(" Unit ");
        STDOUT.print(unit);
        STDOUT.print(" Volume ");
        STDOUT.println(wav_players[unit].volume());
      }
      return true;
    }
#endif
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    if (!strcmp(cmd, "buffered")) {
      for (size_t unit = 0; unit < NELEM(wav_players); unit++) {
        STDOUT.print(" Unit ");
        STDOUT.print(unit);
        STDOUT.print(" Buffered: ");
        STDOUT.println(wav_players[unit].buffered());
      }
      return true;
    }
#endif

#endif // enable sound
    if (!strcmp(cmd, "cd")) {
      chdir(arg);
      SaberBase::DoNewFont();
      return true;
    }
#if 0
    if (!strcmp(cmd, "mkdir")) {
      SD.mkdir(arg);
      return true;
    }
#endif
    if (!strcmp(cmd, "pwd")) {
      for (const char* dir = current_directory; dir; dir = next_current_directory(dir)) {
        STDOUT.println(dir);
      }
      return true;
    }
    if (!strcmp(cmd, "n") || (!strcmp(cmd, "next") && arg && (!strcmp(arg, "preset") || !strcmp(arg, "pre")))) {
      next_preset();
      return true;
    }
    if (!strcmp(cmd, "p") || (!strcmp(cmd, "prev") && arg && (!strcmp(arg, "preset") || !strcmp(arg, "pre")))) {
      previous_preset();
      return true;
    }
    if (!strcmp(cmd, "rotate")) {
      rotate_presets();
      return true;
    }
    if (!strcmp(cmd, "message") && arg) {
      SaberBase::DoMessage(arg);
      return true;
    }

    if (!strcmp(cmd, "list_presets")) {
      CurrentPreset tmp;
      for (int i = 0; ; i++) {
        tmp.SetPreset(i);
        if (tmp.preset_num != i) break;
        tmp.Print();
      }
      return true;
    }

    if (!strcmp(cmd, "set_font") && arg) {
      current_preset_.font = mkstr(arg);
      current_preset_.Save();
      return true;
    }

    if (!strcmp(cmd, "set_track") && arg) {
      current_preset_.track = mkstr(arg);
      current_preset_.Save();
      return true;
    }

    if (!strcmp(cmd, "set_name") && arg) {
      current_preset_.name = mkstr(arg);
      current_preset_.Save();
      return true;
    }

#define SET_STYLE_CMD(N)                             \
    if (!strcmp(cmd, "set_style" #N) && arg) {        \
      current_preset_.current_style##N = mkstr(arg); \
      current_preset_.Save();                        \
      return true;                                   \
    }
    ONCEPERBLADE(SET_STYLE_CMD)

    if (!strcmp(cmd, "move_preset") && arg) {
      int32_t pos = strtol(arg, NULL, 0);
      current_preset_.SaveAt(pos);
      return true;
    }

    if (!strcmp(cmd, "duplicate_preset") && arg) {
      int32_t pos = strtol(arg, NULL, 0);
      current_preset_.preset_num = -1;
      current_preset_.SaveAt(pos);
      return true;
    }

    if (!strcmp(cmd, "delete_preset") && arg) {
      current_preset_.SaveAt(-1);
      return true;
    }

    if (!strcmp(cmd, "show_current_preset")) {
      current_preset_.Print();
      return true;
    }

    if (!strcmp(cmd, "get_preset")) {
      STDOUT.println(current_preset_.preset_num);
      return true;
    }
    if (!strcmp(cmd, "get_volume")) {
#ifdef ENABLE_AUDIO
      STDOUT.println(dynamic_mixer.get_volume());
#else
      STDOUT.println(0);
#endif
      return true;
    }
    if (!strcmp(cmd, "set_volume") && arg) {
#ifdef ENABLE_AUDIO
      int32_t volume = strtol(arg, NULL, 0);
      if (volume >= 0 && volume <= 3000) {
        dynamic_mixer.set_volume(volume);
        PollSaveColorChange();
      }
#endif
      return true;
    }
    if (!strcmp(cmd, "mute")) {
      SetMute(true);
      return true;
    }
    if (!strcmp(cmd, "unmute")) {
      SetMute(false);
      return true;
    }
    if (!strcmp(cmd, "toggle_mute")) {
      if (!SetMute(true)) SetMute(false);
      return true;
    }

    if (!strcmp(cmd, "set_preset") && arg) {
      int preset = strtol(arg, NULL, 0);
      SetPreset(preset, true);
      return true;
    }

    if (!strcmp(cmd, "change_preset") && arg) {
      int preset = strtol(arg, NULL, 0);
      if (preset != current_preset_.preset_num) {
	SetPreset(preset, true);
      }
      return true;
    }

#ifndef DISABLE_COLOR_CHANGE
    if (arg && (!strcmp(cmd, "var") || !strcmp(cmd, "variation"))) {
      size_t variation = strtol(arg, NULL, 0);
      SaberBase::SetVariation(variation);
      return true;
    }
    if (!strcmp(cmd, "ccmode")) {
      ToggleColorChangeMode();
      return true;
    }
#endif

#ifdef ENABLE_SD
    if (!strcmp(cmd, "list_tracks")) {
      // Tracks are must be in: tracks/*.wav or */tracks/*.wav
      LOCK_SD(true);
      ListTracks("tracks");
      for (LSFS::Iterator iter("/"); iter; ++iter) {
        if (iter.isdir()) {
	  PathHelper path(iter.name(), "tracks");
	  ListTracks(path);
	}
      }
      LOCK_SD(false);
      return true;
    }

    if (!strcmp(cmd, "list_fonts")) {
      LOCK_SD(true);
      for (LSFS::Iterator iter("/"); iter; ++iter) {
        if (iter.isdir()) {
          char fname[128];
          strcpy(fname, iter.name());
          strcat(fname, "/");
          char* fend = fname + strlen(fname);
          bool isfont = false;
          if (!isfont) {
            strcpy(fend, "hum.wav");
            isfont = LSFS::Exists(fname);
          }
          if (!isfont) {
            strcpy(fend, "hum01.wav");
            isfont = LSFS::Exists(fname);
          }
          if (!isfont) {
            strcpy(fend, "hum");
            isfont = LSFS::Exists(fname);
          }
          if (isfont) {
            STDOUT.println(iter.name());
          }
        }
      }
      LOCK_SD(false);
      return true;
    }
#endif
    return false;
  }
  void Help() override {
    STDOUT.println(" clash - trigger a clash");
    STDOUT.println(" on/off - turn saber on/off");
    STDOUT.println(" force - trigger a force push");
    STDOUT.println(" blast - trigger a blast");
    STDOUT.println(" stab - trigger a stab");
    STDOUT.println(" lock - begin/end lockup");
    STDOUT.println(" lblock/lb - begin/end lightning block");
    STDOUT.println(" melt - begin/end melt");
#ifdef ENABLE_AUDIO
    STDOUT.println(" pwd - print current directory");
    STDOUT.println(" cd directory - change directory, and sound font");
    STDOUT.println(" play filename - play file");
    STDOUT.println(" next/prev font - walk through directories in alphabetical order");
    STDOUT.println(" next/prev pre[set] - walk through presets.");
    STDOUT.println(" beep - play a beep");
#endif
  }

  virtual bool Event(enum BUTTON button, EVENT event) {
    PrintEvent(button, event);

    switch (event) {
      case EVENT_RELEASED:
        clash_pending_ = false;
      case EVENT_PRESSED:
        IgnoreClash(50); // ignore clashes to prevent buttons from causing clashes
      default:
        break;
    }

    if (Event2(button, event, current_modifiers | (IsOn() ? MODE_ON : MODE_OFF))) {
      current_modifiers = 0;
      return true;
    }
    if (Event2(button, event,  MODE_ANY_BUTTON | (IsOn() ? MODE_ON : MODE_OFF))) {
      // Not matching modifiers, so no need to clear them.
      current_modifiers &= ~button;
      return true;
    }
    return false;
  }

  virtual bool Event2(enum BUTTON button, EVENT event, uint32_t modifiers) = 0;

protected:
  CurrentPreset current_preset_;
  LoopCounter accel_loop_counter_;
};

#endif
