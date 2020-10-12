#ifndef SCRIPTS_CLASH_RECORDER_H
#define SCRIPTS_CLASH_RECORDER_H

class ClashRecorder : public SaberBase {
public:
  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      default: return;
      case EFFECT_CLASH:
      case EFFECT_STAB:
	time_to_dump_ = NELEM(buffer_) / 2;
	STDOUT.println("dumping soon...");
	SaberBase::RequestMotion();
    }
	
  }
  void SB_Accel(const Vec3& accel, bool clear) override {
    SaberBase::RequestMotion();
    loop_counter_.Update();
    buffer_[pos_] = accel;
    pos_++;
    if (pos_ == NELEM(buffer_)) pos_ = 0;
    if (!time_to_dump_) return;
    time_to_dump_--;
    if (time_to_dump_) return;

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
    for (size_t i = 0; i < NELEM(buffer_); i++) {
      const Vec3& v = buffer_[(pos_ + i) % NELEM(buffer_)];
      f.print(v.x);
      f.print(", ");
      f.print(v.y);
      f.print(", ");
      f.print(v.z);
      f.print("\n");
    }
    f.close();
    LOCK_SD(false);
    STDOUT.print("Clash dumped to ");
    STDOUT.print(file_name);
    STDOUT.print(" ~ ");
    loop_counter_.Print();
    STDOUT.println(" measurements / second");
  }
private:
  size_t last_file_ = 0;
  size_t time_to_dump_ = 0;
  size_t pos_ = 0;
  Vec3 buffer_[512];
  LoopCounter loop_counter_;
};

#ifdef CLASH_RECORDER
ClashRecorder clash_recorder;
void DumpClash() { clash_recorder.SB_Clash(); }
#else
void DumpClash() {}
#endif

#endif

