#ifndef SCRIPTS_CLASH_RECORDER_H
#define SCRIPTS_CLASH_RECORDER_H

// To use, simply include this file *after* your normal prop include in CONFIG_PROP, like so:
//
// #ifdef CONFIG_PROP
// #include "../props/saber.h"
// #include "../scripts/motion_startup_recorder.h"
// #endif
//
// Files will be saved on your SD card called M0000000.CSV, M0000001.CSV etc, every time
// the motion chip wakes up.

template<class BASE>
class MotionRecorder : public PROP_INHERIT_PREFIX BASE {
public:
  enum STATE {
    START,
    RECORD,
    SAVE
  };
  MotionRecorder() {
    Clear();
  }
  void Clear() {
    memset(accel_, 0, sizeof(accel_));
    memset(gyro_, 0, sizeof(gyro_));
    memset(swing_speed_, 0, sizeof(swing_speed_));
  }
  
  virtual void DoAccel(const Vec3& accel, bool clear) {
    PropBase::DoAccel(accel, clear);
    if (clear && state_ == START) {
      pos_ = 0;
      state_ = RECORD;
    }
    if (state_ != RECORD) return;
    accel_[pos_] = accel;
  }
  virtual void DoMotion(const Vec3& gyro, bool clear) {
    BASE::DoMotion(gyro, clear);
    if (state_ != RECORD) return;
    gyro_[pos_] = gyro;
    pos_++;
    if (pos_ == NELEM(accel_)) {
      state_ = SAVE;
    }
  }

  void Save() {
    LOCK_SD(true);
    char file_name[16];
    size_t file_num = last_file_ + 1;
    while (true) {
      char num[16];
      itoa(file_num, num, 10);
      strcpy(file_name, "M");
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
    for (size_t i = 0; i < NELEM(accel_); i++) {
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
      f.print(swing_speed_[i]);
      f.print("\n");
    }
    f.close();
    LOCK_SD(false);
    STDOUT << "Motion data dumped to " << file_name << ".\n";
  }

  void Loop() override {
    BASE::Loop();

    switch (state_) {
      case START:
	break;
      case RECORD: {
	  size_t p = pos_;
	  if (p < NELEM(accel_)) {
	    if (fusor.ready()) {
	      swing_speed_[p] = fusor.swing_speed();
	    } else {
	      swing_speed_[p] = -1.0;
	    }
	  }
	}
	break;
      case SAVE:
	Save();
        Clear();
	state_ = START;
    }
  }
private:
  volatile STATE state_ = STATE::START;
  volatile size_t pos_ = 0;
  Vec3 accel_[256];
  Vec3 gyro_[256];
  float swing_speed_[256];
  
  size_t last_file_ = 0;
};

using PROP_TYPE_TMP = PROP_TYPE;
#undef PROP_TYPE
#define PROP_TYPE MotionRecorder<PROP_TYPE_TMP>

#endif
