#ifndef SCRIPTS_CLASH_RECORDER_H
#define SCRIPTS_CLASH_RECORDER_H

// Use clash_recorder_config.h to include in your config file.

template<class BASE>
class ClashRecorder : public PROP_INHERIT_PREFIX BASE, StateMachine {
public:
  virtual void DoAccel(const Vec3& accel, bool clear) {
    PropBase::DoAccel(accel, clear);
    if (do_save_) return;
    accel_[pos_] = accel;
    down_[pos_] = fusor.down();
    if (!save_countdown_ && BASE::interrupt_clash_pending()) {
      save_countdown_ = NELEM(accel_) * 3 / 4;
    }
  }
  virtual void DoMotion(const Vec3& gyro, bool clear) {
    BASE::DoMotion(gyro, clear);
    if (do_save_) return;
    gyro_[pos_] = gyro;
    pos_++;
    if (pos_ == NELEM(accel_)) pos_ = 0;
    if (save_countdown_) {
      if (!--save_countdown_) {
	do_save_ = true;
      }
    }
  }

  void On() override {
    ArgParser ap("");
    CurrentArgParser = &ap;
    free(current_config->blade1->UnSetStyle());
    current_config->blade1->SetStyle(new Style<ClashRecorderStyle>());

    BASE::On();
  }

  bool Event(enum BUTTON button, EVENT event) override {
    if (capture_button_) {
      STDOUT << "Capturing event:\n";
      BASE::PrintEvent(button, event);
      switch (event) {
	default: break;
	case EVENT_CLICK_SHORT:
	case EVENT_FIRST_CLICK_SHORT:
	case EVENT_SECOND_CLICK_SHORT:
	case EVENT_THIRD_CLICK_SHORT:
	case EVENT_FOURTH_CLICK_SHORT:
	  last_button_ = button;
	  capture_button_ = false;
      }
      return true;
    }
    return BASE::Event(button, event);
  }

  void Loop() override {
    BASE::Loop();

    STATE_MACHINE_BEGIN();
    while (true) {
      while (!BASE::IsOn()) YIELD();
      SLEEP(400);
      do_save_ = false;
      SLEEP(200);
      location_ = (50 + random(940));
      ShowLocationF::SetPos(location_ * 32767 / 1000);
      SaberBase::DoEffect(EFFECT_USER1, location_);

      while (!do_save_) YIELD();
      talkie.Say(spTO);
      talkie.Say(spUSE);
      talkie.Say(spPRESS);
      talkie.Say(spPOWER);

      capture_button_ = true;
      last_button_ = BUTTON_NONE;
      while (capture_button_ && BASE::IsOn()) {
	YIELD();
      }
      capture_button_ = false;

      if (last_button_ != BUTTON_POWER) {
	talkie.Say(spCANCEL);
	continue;
      }

      talkie.Say(spFILED);

      {
	LOCK_SD(true);
	char file_name[16];
	size_t file_num = last_file_ + 1;
	while (true) {
	  char num[16];
	  itoa(file_num, num, 10);
	  strcpy(file_name, "CLS");
	  while(strlen(num) + strlen(file_name) < 8) strcat(file_name, "0");
	  strcat(file_name, num);
	  strcat(file_name, ".CSV");
          
	  int last_skip = file_num - last_file_;
	  if (LSFS::Exists(file_name)) {
	    last_file_ = file_num;
	    file_num += last_skip * 2;
	    continue;
	  }

	  if (file_num - last_file_ > 1) {
	    file_num = last_file_ + last_skip / 2;
	    continue;
	  }
	  break;
	}

	File f = LSFS::OpenForWrite(file_name);
	f.print("position,");
	f.println(location_);
	for (size_t j = 0; j < NELEM(accel_); j++) {
	  int i = (pos_ + j) % NELEM(accel_);
	  f.print(accel_[i].x);
	  f.print(", ");
	  f.print(accel_[i].y);
	  f.print(", ");
	  f.print(accel_[i].z);
	  f.print(", ");
	  f.print(gyro_[i].x);
	  f.print(", ");
	  f.print(gyro_[i].y);
	  f.print(", ");
	  f.print(gyro_[i].z);
	  f.print(", ");
	  f.print(down_[i].x);
	  f.print(", ");
	  f.print(down_[i].y);
	  f.print(", ");
	  f.print(down_[i].z);
	  f.print("\n");
	}
	f.close();
	LOCK_SD(false);
	STDOUT << "Clash dumped to " << file_name << ".\n";
      }
    }
    STATE_MACHINE_END();
  }
private:
  bool capture_button_ = false;
  enum BUTTON last_button_ = BUTTON_NONE;
  size_t pos_ = 0;
  int save_countdown_ = 0;
  Vec3 accel_[256];
  Vec3 gyro_[256];
  Vec3 down_[256];
  volatile bool do_save_;
  float location_;
  
  size_t last_file_ = 0;

  ShowColorAllBladesTemplate<ShowColorStyle> show_color_all_;
};

#endif
