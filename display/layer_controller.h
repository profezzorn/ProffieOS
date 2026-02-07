#ifndef DISPLAY_LAYER_CONTROLLER_H
#define DISPLAY_LAYER_CONTROLLER_H

#include "../sound/audio_stream_work.h"

/*

Layer Controller: Tells displays what to do.

The layer controller looks for the following files:

   WxH/boot.scr
   WxH/font.scr
   WxH/blst.scr
   WxH/clsh.scr
   WxH/force.scr
   WxH/preon.scr
   WxH/out.scr
   WxH/in.scr
   WxH/pstoff.scr
   WxH/on.scr
   WxH/lock.scr
   WxH/pli.scr
   WxH/idle.scr
   WxH/lowbatt.scr

W and H stands for the Width and Height of the display respectively.
Standard EFFECT rules are used to find these files, so here are
some examples of actual scr file names:

  128x80/boot.scr
  128x80/boot04.scr
  128x80/boot/004.scr
  alt004/128x80/boot/004.scr
  128x80/boot/004/001.scr

When an event occurs, the SCR file is read. SCR files are text files
which specifies what the display should do. Each line in the SCR file
should contain one of:

  layer                 # this advances to the next layer
  file=boot128x80.pqf   # Tells the current layer to start playing boot128x80.pqf
  restart               # start file from beginning
  time=1000             # Play this file 1000ms, then stop
  A=battery             # Variable assignment

Note that if "restart" is not present, and you specify the same filename as what
is already playing, it just continues to play.

For variable assignments, possible variables are A, B, C, D, E, and currently
possible variable values are "battery" and "volume".

Here is an example SCR file which uses two layers:
--------------------------
file=sw.pqf
layer
file=batt128x80.pqf
A=battery
--------------------------

Note that base layer files MUST NOT be transparent, but files on other layers
can be transparent or not. Of course permanently displaying a non-transparent
file on a layer other than the first one makes no sense, but it is possible.

Note that if "time" is not present, the file will play to it's end, or forever
if it is looped.

*/

#include "../common/bytearray.h"

struct VariableSource {
  virtual int percent() = 0;
};

struct BatteryVariableSource : public VariableSource {
  int percent() override { return (int)battery_monitor.battery_percent(); }
};
BatteryVariableSource battery_variable_source;

struct SoundLevelVariableSource : public VariableSource {
  int percent() override {
#ifdef ENABLE_AUDIO
    return clampi32(sqrtf(dynamic_mixer.audio_volume()) / 1638, 0, 100);
#else
    return 0;
#endif
  }
};

SoundLevelVariableSource sound_level_variable_source;

struct VolumeVariableSource : public VariableSource {
  int percent() override {
#ifdef ENABLE_AUDIO
    return clampi32(dynamic_mixer.get_volume() * 100 / VOLUME, 0, 100);
#else
    return 0;
#endif
  }
};

VolumeVariableSource volume_variable_source;

struct NULLVariableSource : public VariableSource {
  int percent() override { return 0; }
};

NULLVariableSource null_variable_source;

class LayerControl {
public:
  virtual void LC_setVariable(int variable, VariableSource* variable_source) = 0;
  virtual const char* LC_get_filename() = 0;
  virtual void LC_restart() = 0;
  virtual void LC_play(const char* filename) = 0;
  virtual void LC_set_time(uint32_t millis) = 0;
};

class LayerControllerInterface {
public:
  virtual void LC_onStop() = 0;
  virtual bool LC_done() = 0;
};

class LayeredScreenControl {
public:
  virtual LayerControl* getLayer(int layer) = 0;
  virtual void LSC_SetController(LayerControllerInterface* lci) = 0;
  virtual void LSC_Top() = 0;
};

template<int W, int H>
class SizedLayeredScreenControl : public LayeredScreenControl {};

class BufferedFileReader : public AudioStreamWork {
protected:
  void SEEK_LOW(uint32_t pos) {
    TRACE2(RGB565, "SEEK_LOW", pos);
    stream_locked_.set(true);
    do_seek_ = true;
    seek_pos_ = pos;
    input_buffer_.clear();
    stream_locked_.set(false);
  }

  void SEEK(uint32_t pos) {
    TRACE2(RGB565, "SEEK", pos);
    if (pos > TELL() && pos - TELL() < input_buffer_.size()) {
      // Short forward seek within our buffer, just pop the data.
      input_buffer_.pop(pos - TELL());
    } else {
      SEEK_LOW(pos);
    }
  }

  uint32_t TELL() {
    uint32_t ret = seek_pos_ + input_buffer_.pos();
    // TRACE2(RGB565, "TELL", ret);
    return ret;
  }

  // Returns true if all the data in the file has been consumed.
  bool ATEOF() {
    // TRACE2(RGB565, "ATEOF", file_.get_do_open());
    if (file_.get_do_open()) return false;
    if (file_size_ == 0xFFFFFFFFU) return true;
    return TELL() == file_size_;
  }

  // Returns true if the buffered data reaches the end of the file.
  bool BUFATEOF() {
    // TRACE2(RGB565, "ATEOF", file_.get_do_open());
    if (file_.get_do_open()) return false;
    if (file_size_ == 0xFFFFFFFFU) return true;
    return TELL() + input_buffer_.size() == file_size_;
  }

  bool FillBuffer() override {
    SCOPED_PROFILER();
    TRACE2(RGB565_DATA, "FillBuffer, fp=", file_.Tell());
    TRACE2(RGB565_DATA, "FillBuffer, tell=", TELL());
    if (stream_locked_.get()) {
      TRACE(RGB565_DATA, "FillBuffer, locked");
      return false;
    }
    if (!input_buffer_.space_available()) {
      TRACE(RGB565_DATA, "FillBuffer, no space available");
      return false;
    }
    if (file_.OpenFile()) {
      input_buffer_.clear();
      seek_pos_ = 0;
      if (!file_.IsOpen()) {
        file_size_ = 0;
        TRACE(RGB565, "FillBuffer::EOF");
        return false;
      }
      file_size_ = file_.FileSize();
      // TRACE(RGB565, "FillBuffer5");
      return true;  // return to ProcessAudioStream which will return here when no audiostreams are in need of data.
    }
    // TRACE(RGB565, "FillBuffer5.5");
    if (BUFATEOF()) return false;
    if (do_seek_) {
      TRACE2(RGB565, "FillBuffer, seek to ", seek_pos_);
      file_.Seek(seek_pos_);
      do_seek_ = false;
      // TRACE(RGB565, "FillBuffer6");
      return true;
    }
    uint32_t toread = input_buffer_.continuous_space();
    uint32_t max_read = 512 - (file_.Tell() % 512);  // Read to end of block
    TRACE2(RGB565_DATA, "FillBuffer, toread=", std::min(toread, max_read));
    uint32_t bytes_read = file_.Read(input_buffer_.space(), std::min(toread, max_read));
    TRACE2(RGB565_DATA, "FillBuffer, bytes_read=", bytes_read);

    uint32_t now = millis();
    uint32_t m = now - bps_last_millis_;
    if (m > 1000) {
      bps_bytes_ = 0;
      bps_millis_ = 0;
    } else {
      bps_bytes_ += bytes_read;
      bps_millis_ += m;
      if (bps_millis_ > 5000) {
        bps_millis_ /= 2;
        bps_bytes_ /= 2;
      }
    }
    bps_last_millis_ = now;

    input_buffer_.push(bytes_read);
    TRACE2(RGB565_DATA, "FillBuffer, bufsize=", input_buffer_.size());
    return true;
  }

  float kbps() {
    if (bps_millis_ == 0) return 0.0f;
    return bps_bytes_ / (float)bps_millis_;
  }


  void CloseFiles() override {
    TRACE(RGB565, "CloseFiles");
    file_.Close();
    file_size_ = 0xFFFFFFFFU;
  }

  size_t space_available() override {
    TRACE2(RGB565_DATA, "space_available this=", (uint32_t)this);
    if (input_buffer_.space_available() < 512) {
      TRACE2(RGB565_DATA, "space_available ib=", input_buffer_.space_available());
      return 0;
    }
    // if (!input_buffer_.space_available()) return 0;
    if (stream_locked_.get()) {
      TRACE2(RGB565_DATA, "space_available locked=", stream_locked_.get());
      return 0;
    }

    // TRACE2(RGB565_DATA, "file_.get_do_open()=", file_.get_do_open());
    // TRACE2(RGB565_DATA, "file_size_=", file_size_);
    // TRACE2(RGB565_DATA, "TELL()=", TELL());
    // TRACE2(RGB565_DATA, "ib.size=", input_buffer_.size());

    if (BUFATEOF()) {
      TRACE2(RGB565_DATA, " BUFATEOF=", BUFATEOF());
      return 0;
    }
    // TRACE(RGB565_DATA, " return 1");

    // Always low priority
    return 1;
  }

  int getc() { return input_buffer_.pop(); }

  uint32_t bps_bytes_ = 0;
  uint32_t bps_millis_ = 0;
  uint32_t bps_last_millis_ = 0;

  POLYHOLE;
  POAtomic<bool> stream_locked_;
  volatile uint32_t file_size_ = 0xFFFFFFFFU;
  volatile bool do_seek_;  // atomic?
  uint32_t seek_pos_ = 0;
  POLYHOLE;
  CircularBuffer<uint8_t, 1024> input_buffer_;
  POLYHOLE;
  EffectFileReader file_;
  POLYHOLE;
};

class SCRReader : public BufferedFileReader {
public:
  explicit SCRReader(LayeredScreenControl* screen) : screen_(screen) {
    state_machine_.stop();
  }
  // TODO: Allow the prop to supply variable sources.
  VariableSource* parse_variable_source(const char* variable_source) {
    if (!strcmp(variable_source, "battery")) return &battery_variable_source;
    if (!strcmp(variable_source, "volume")) return &volume_variable_source;
    if (!strcmp(variable_source, "sound_level")) return &sound_level_variable_source;
    STDERR << "Unknown variable source: " << variable_source << "\n";
    return &null_variable_source;
  }

#if 1
  void loop() {
    STATE_MACHINE_BEGIN();
    layer_number_ = 0;
    may_need_restart_ = false;
    layer_time_ = 0;
    layer_ = screen_->getLayer(layer_number_);

    while (true) {
      for (i = 0; i < sizeof(line); i++) {
        TRACE2(RGB565, "line", i);
        while (input_buffer_.empty()) {
          TRACE2(RGB565, "empty", i);
          if (ATEOF()) {
            if (i) {
              line[i] = 0;
              goto have_line;
            }
            TRACE2(RGB565, "EOF", TELL());
            goto done;
          }
          scheduleFillBuffer();
          YIELD();
        }
        c = getc();
        if (c == '\r' || c == '\n') {
          line[i] = 0;
          break;
        }
        line[i] = c;
      }

    have_line:
      PVLOG_VERBOSE << "PARSING SCR LINE '" << line << "'\n";

      char* tmp = strchr(line, '#');
      if (tmp) {
        *tmp = 0;
        while (tmp > line && isspace(tmp[-1])) { *--tmp = 0; }
      }
      char* eq = strchr(line, '=');
      char* key = line;
      char* value = 0;
      if (eq) {
        *eq = 0;
        value = eq + 1;
      }
      if (!strcmp(key, "layer")) {
        may_need_restart_ = false;
        layer_time_ = 0;
        layer_ = screen_->getLayer(++layer_number_);
        if (!layer_) {
          STDERR << "Too many layers.\n";
          break;
        }
      } else if (!strcmp(key, "file")) {
        // Filename is relative to the SCR file.
        PathHelper path(file_.GetFilename());
        path.Dirname();
        path.Append(value);

        if (!strcmp(layer_->LC_get_filename(), path)) {
          // Same file.
          if (do_restart_) {
            layer_->LC_restart();
          } else {
            may_need_restart_ = true;
          }
        } else {
          layer_->LC_play(path);
          layer_->LC_set_time(layer_time_);
        }
      } else if (!strcmp(key, "restart")) {
        if (may_need_restart_) {
          layer_->LC_restart();
          may_need_restart_ = false;
        } else {
          do_restart_ = true;
        }
      } else if (!strcmp(key, "time")) {
        if (!strcmp(value, "from_sound")) {
          layer_time_ = sound_time_ms_;
        } else {
          layer_time_ = atoi(value);
          layer_->LC_set_time(layer_time_);
        }
        layer_->LC_set_time(layer_time_);
      } else if (!strcmp(key, "A") ||
                 !strcmp(key, "B") ||
                 !strcmp(key, "C") ||
                 !strcmp(key, "D") ||
                 !strcmp(key, "E")) {
        layer_->LC_setVariable(key[0] - 'A', parse_variable_source(value));
      }
    }
  done:
    active_ = false;
    TRACE(RGB565, "done");
    STATE_MACHINE_END();
    check_open();
  }
#else
  void loop() {}
#endif

  bool Play(Effect* f) {
    if (!*f) return false; // no files, do nothing
    PVLOG_VERBOSE << "SCR Playing " << f->GetName() << "\n";
    wait_for_previous_open_to_complete();
    sound_time_ms_ = SaberBase::sound_length * 1000;
    file_.PlayInternal(f);
    delayed_open_ = true;
    check_open();
    return true;
  }

  void Play(const char* filename) {
    wait_for_previous_open_to_complete();
    sound_time_ms_ = SaberBase::sound_length * 1000;
    file_.PlayInternal(filename);
    delayed_open_ = true;
    check_open();
  }

  LayeredScreenControl* screen() { return screen_; }

  void dumpstate() {
    STDOUT << "next state: " << state_machine_.next_state_ << "\n";
  }

  bool IsActive() override { return active_ || delayed_open_; }

protected:
  void wait_for_previous_open_to_complete() {
    while (delayed_open_ && !state_machine_.done()) {
      loop();
      check_open();
    }
  }

  void check_open() {
    if (delayed_open_ && state_machine_.done()) {
      active_ = true;
      file_.do_open();
      TRACE2(RGB565, "DOOPEN", ATEOF());
      delayed_open_ = false;
      scheduleFillBuffer();
      TRACE(RGB565, "Reset State Machine");
      state_machine_.reset_state_machine();
    }
  }

  size_t i, c;
  bool do_restart_;

  bool delayed_open_ = false;
  char line[128+32+5];
  bool may_need_restart_;
  uint32_t layer_time_ = 0;
  LayeredScreenControl* screen_;
  int layer_number_;
  LayerControl* layer_ = nullptr;
  int sound_time_ms_;
  bool active_ = false;

private:
  StateMachineState state_machine_;
};

#define ONCE_PER_EFFECT(X) \
  X(boot)                  \
  X(font)                  \
  X(bladein)               \
  X(bladeout)              \
  X(blst)                  \
  X(clsh)                  \
  X(force)                 \
  X(preon)                 \
  X(out)                   \
  X(in)                    \
  X(pstoff)                \
  X(on)                    \
  X(lock)                  \
  X(pli)                   \
  X(idle)                  \
  X(lowbatt)

#define INIT_SCR(X, ARGS...) ,SCR_##X(ConcatByteArrays<PREFIX, ByteArray<'/'>, STRTYPE(#X)>::str, nullptr, Effect::FileType::SCREEN)
#define DEF_SCR(X, ARGS...) Effect SCR_##X;

template<int W, int H, typename PREFIX = ConcatByteArrays<typename NumberToByteArray<W>::type, ByteArray<'x'>, typename NumberToByteArray<H>::type>>
class StandardColorDisplayController : public SaberBase, public Looper, public CommandParser, public LayerControllerInterface {
public:
//  typedef SizedLayeredScreenControl<W, H> SLSC;
//  explicit StandardColorDisplayController(SLSC* screen) : scr_(screen) ONCE_PER_EFFECT(INIT_SCR) {}

  template<int w, int h>
  explicit StandardColorDisplayController(SizedLayeredScreenControl<w, h>* screen) : scr_(screen) ONCE_PER_EFFECT(INIT_SCR) {
    static_assert(w == W, "Width is not matching.");
    static_assert(h == H, "Height is not matching.");
    screen->LSC_SetController(this);
  }

  void SB_On2(EffectLocation location) override {
    if (!scr_.Play(&SCR_out)) {
      ShowDefault();
    }
  }

  void SB_Top(uint64_t total_cycles) override { scr_.screen()->LSC_Top(); }

  void SB_Off2(OffType offtype, EffectLocation location) override {
    if (offtype == OFF_IDLE) {
      Stop();
    } else if (!scr_.Play(&SCR_in)) {
      ShowDefault();
    }
  }

  void LC_onStop() override {
    PVLOG_VERBOSE << "LC_onStop()\n";
    ShowDefault();
  }

  bool LC_done() override {
    return !scr_.IsActive();
  }

  const char* name() override { return "ColorDisplayController"; }

#if 0
  void usb_connected() override {
    if (EscapeIdleIfNeeded() && current_effect_ == &SCR_idle) {
      // We are idle-looping, and usb is connected. Time to stop.
      SetMessage("    usb\nconnected");
      SetScreenNow(SCREEN_MESSAGE);
    }
  }
#endif

  enum class Tristate : uint8_t {
    Unknown,
    True,
    False
  };

  void Stop() {
    PVLOG_VERBOSE << "LC Stop()\n";
    scr_.Play("");
    for (int i = 0;; i++) {
      LayerControl *layer = scr_.screen()->getLayer(i);
      if (!layer) break;
      layer->LC_play("");
    }
  }

  virtual void ShowDefault(bool ignore_lockup = false) {
    PVLOG_VERBOSE << "LC ShowDefault()\n";
    if (SaberBase::IsOn()) {
      if (SaberBase::Lockup() && SCR_lock && !ignore_lockup) {
        scr_.Play(&SCR_lock);
      } else if (looped_on_ != Tristate::False) {
        scr_.Play(&SCR_on);
      }
    } else {
      // Off
      if (!AvoidIdleSDAccess()) {
        scr_.Play(&SCR_idle);
      }
    }
  }

  void Loop() override {
    scr_.loop();
    // Need to call ShowDefault when the previous effect is over.
  }

  void SB_Effect2(EffectType effect, EffectLocation location) override {
    switch (effect) {
      case EFFECT_CHDIR:
        looped_on_ = Tristate::Unknown;
        Stop();
        break;
      case EFFECT_BLADEIN:
        if (scr_.Play(&SCR_bladeout)) break;
        if (scr_.Play(&SCR_font)) break;
        ShowDefault();
        break;
      case EFFECT_BLADEOUT:
        if (scr_.Play(&SCR_bladein)) break;
        /* fall through */
      case EFFECT_NEWFONT:
        if (scr_.Play(&SCR_font)) break;
        ShowDefault();
        break;
      case EFFECT_LOCKUP_BEGIN:
        ShowDefault();
        break;
      case EFFECT_LOCKUP_END:
        ShowDefault(true);
        break;
      case EFFECT_BATTERY_LEVEL:
        // Show On-Demand battery meter
        scr_.Play(&SCR_pli);
        break;
      case EFFECT_LOW_BATTERY:
        scr_.Play(&SCR_lowbatt);
        break;
      case EFFECT_BOOT:
        if (!scr_.Play(&SCR_boot)) {
          ShowDefault();
        }
        break;
      case EFFECT_BLAST:
        scr_.Play(&SCR_blst);
        break;
      case EFFECT_CLASH:
        scr_.Play(&SCR_clsh);
        break;
      case EFFECT_FORCE:
        scr_.Play(&SCR_force);
        break;
      case EFFECT_PREON:
        scr_.Play(&SCR_preon);
        break;
      case EFFECT_POSTOFF:
        scr_.Play(&SCR_pstoff);
        break;
      default: break;
    }
  }

  bool iscmd(const char* command, LayerControl **layer, const char* cmd) {
    int layer_number = 0;
    size_t l = sizeof(PREFIX::str);
    if (!strncmp(cmd, PREFIX::str, l) && cmd[l]=='_') {
      cmd += l + 1;
    }
    if (*cmd == 'L') {
      char* x = nullptr;
      layer_number = strtol(cmd+1, &x, 10);
      if (x) cmd += x - cmd;
      if (*cmd != '_') return false;
      cmd++;
    }
    *layer = scr_.screen()->getLayer(layer_number);
    return strcmp(command, cmd) == 0;
  }

  bool Parse(const char* cmd, const char* arg) override {
#ifndef DISABLE_DIAGNOSTIC_COMMANDS
    LayerControl* layer = nullptr;
    if (iscmd("show", &layer, cmd) && arg) {
      PVLOG_STATUS << "Showing " << arg << "\n";
      if (endswith(".scr", arg)) {
        scr_.Play(arg);
      } else {
        layer->LC_play(arg);
      }
      return true;
    }
    if (!strcmp(cmd, "displaycontrollerstate")) {
      scr_.dumpstate();
      return true;
    }
#endif
    return false;
  }

protected:
  SCRReader scr_;
  ONCE_PER_EFFECT(DEF_SCR)

  // TODO: There is currently no easy way to know if a PQF files is looped...

  // True if IMG_on is looped.
  volatile Tristate looped_on_ = Tristate::Unknown;
};

//#undef ONCE_PER_EFFECT
//#undef INIT_SCR
//#undef DEF_SCR

#endif  // DISPLAY_LAYER_CONTROLLER_H

