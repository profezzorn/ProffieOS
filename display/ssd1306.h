#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H

#ifndef PLI_OFF_TIME
#define PLI_OFF_TIME ((font_config.ProffieOSFontImageDuration == 0) ? 3500 : font_config.ProffieOSFontImageDuration)
#endif

#include "monoframe.h"

// Images/animations
IMAGE_FILESET(boot);
IMAGE_FILESET(font);
IMAGE_FILESET(blst);
IMAGE_FILESET(clsh);
IMAGE_FILESET(force);
IMAGE_FILESET(preon);
IMAGE_FILESET(out);
IMAGE_FILESET(in);
IMAGE_FILESET(pstoff);
IMAGE_FILESET(on);
IMAGE_FILESET(lock);
IMAGE_FILESET(idle);
/* To-Do, possibly differently
#ifdef OLED_USE_BLASTER_IMAGES
IMAGE_FILESET(reload);
IMAGE_FILESET(empty);
IMAGE_FILESET(jam);
IMAGE_FILESET(clipin);
IMAGE_FILESET(clipout);
IMAGE_FILESET(destruct);
#endif
*/
IMAGE_FILESET(lowbatt);

enum Screen {
  SCREEN_UNSET,
  SCREEN_STARTUP,
  SCREEN_MESSAGE,
  SCREEN_ERROR_MESSAGE,
  SCREEN_PLI,
  SCREEN_IMAGE,  // also for animations
  SCREEN_OFF,
  SCREEN_DEFAULT
};

template<int Width, class col_t>
class Display : public MonoFrame<Width, col_t> {
public:
  virtual void Page() = 0;
  virtual void SB_Top() = 0;
  virtual Screen GetScreen() = 0;
  virtual void SB_IsOn(bool* on) = 0;
};

template<int Width, class col_t>
class DisplayControllerBase {
public:
  // Return zero if frame is not yet ready.
  // May be called before it's time to actually display next frame.
  virtual int FillFrameBuffer(bool advance) = 0;
  virtual void SetDisplay(Display<Width, col_t>* display) = 0;
  virtual Screen GetScreen() { return SCREEN_UNSET; }
  virtual void usb_connected() = 0;
};

template<int Width, class col_t>
class AbstractDisplayController : public DisplayControllerBase<Width, col_t> {
public:
  Display<Width, col_t>* display_;
  void SetDisplay(Display<Width, col_t>* display) override {
    display_ = display;
  }
  bool ImageScreen() { return display_ && display_->GetScreen() == SCREEN_IMAGE; }
};

template<int Width, class col_t>
class DisplayHelper1 {
public:
  DisplayControllerBase<Width, col_t> *a;
  int t_;
  int FillFrameBuffer() {
    if (t_ <= 0) {
      t_ += a->FillFrameBuffer(true);
    } else {
      a->FillFrameBuffer(false);
    }
    return t_;
  }
  void SetDisplay(Display<Width, col_t>* display)  {
    a->SetDisplay(display);
  }
  Screen GetScreen() override { return a->GetScreen(); }
  void usb_connected() override { a->usb_connected(); }
  void Advance(int t) { t_ -= t; }
};

template<int Width, class col_t, class... OPS> class DisplayHelper2 {};
template<int Width, class col_t>
class DisplayHelper2<Width, col_t> {
 public:
  void set(DisplayHelper1<Width, col_t>* out) { }
};

template<int Width, class col_t, class OP, class... OPS>
class DisplayHelper2<Width, col_t, OP, OPS...> {
 public:
  void set(DisplayHelper1<Width, col_t>* out) {
    out->a = &op_;
    rest_.set(out + 1);
  }
 private:
  typename OP::template Controller<Width, col_t> op_;
  DisplayHelper2<Width, col_t, OPS...> rest_;
};

template<int Width, class col_t, class... OPERATIONS>
class DisplayHelper : public DisplayControllerBase<Width, col_t> {
public:
  int last_t_;
  DisplayHelper1<Width, col_t> operations_[sizeof...(OPERATIONS)];
  DisplayHelper2<Width, col_t, OPERATIONS ...> ops_;

  DisplayHelper() {
    ops_.set(operations_);
  }
  void SetDisplay(Display<Width, col_t>* display) override {
    for (size_t i = 0; i < sizeof...(OPERATIONS); i++) {
      operations_[i].SetDisplay(display);
    }
  }
  Screen GetScreen() override {
    Screen ret;
    for (size_t i = 0; i < sizeof...(OPERATIONS); i++) {
      ret = operations_[i].GetScreen();
      if (ret != SCREEN_UNSET) return ret;
    }
    return SCREEN_UNSET;
  }
  void usb_connected() override {
    for (size_t i = 0; i < sizeof...(OPERATIONS); i++) {
      operations_[i].usb_connected();
    }
  }

  int FillFrameBuffer(bool advance) override {
    for (size_t i = 0; i < sizeof...(OPERATIONS); i++) {
      operations_[i].Advance(last_t_);
    }
    last_t_ = 10000;
    for (size_t i = 0; last_t_ && i < sizeof...(OPERATIONS); i++) {
      last_t_ = std::min(last_t_, operations_[i].FillFrameBuffer());
    }
    return last_t_;
  }
};


// Operations

// Clear entire screen.
// Intended to be used as base layer.
struct ClearScreenOp {
  template<int Width, class col_t> struct Controller : public AbstractDisplayController<Width, col_t> {
    Screen GetScreen() override { return SCREEN_IMAGE; }
    int FillFrameBuffer(bool advance) override {
      this->display_->ClearRect(0, Width, 0, sizeof(col_t) * 8);
      return 1000; // Never changes.
    }
  };
};


// Op wrapper, skips the op if the base is not an image.
template<template<int, class> class T>
struct IfImageOp {
  template<int Width, class col_t> struct Controller : public T<Width, col_t> {
    int FillFrameBuffer(bool advance) override {
      if (!this->ImageScreen()) return 1000;
      return typename T<Width, col_t>::FillFrameBuffer(advance);
    }
  };
};


// Clear a rectangle.
template<int x1, int x2, int y1, int y2> struct ClearRectangleOp {
  template<int Width, class col_t> struct Controller : public AbstractDisplayController<Width, col_t> {
    int FillFrameBuffer(bool advance) override {
      this->display_->ClearRect(x1, x2, y1, y2);
      return 1000; // Never changes.
    }
  };
};

// TODO: Figure out how to replace this ifdef with std::enable_if
#ifdef PROP_HAS_BULLET_COUNT

// Draw bullet count on the screen.
// May need to be combined with a ClearRectangleOp
template<int x, int y, int digits> struct WriteBulletCountOp {
  template<int Width, class col_t> struct Controller : public AbstractDisplayController<Width, col_t> {
    int FillFrameBuffer(bool advance) override {
      char tmp[30];
      for (int i = 0; i < digits; i++) tmp[i] = '0';
      itoa(prop.GetBulletCount(), tmp+digits, 10);
      this->display_->DrawText(tmp + std::min<int>(digits, strlen(tmp+digits)), x, y, Starjedi10pt7bGlyphs);
      return 50; // new frame in 50 ms or less.
    }
  };
};

#endif



#if 0
class DisplayHelper {
  DisplayControllerBase *a;
  int t_;
  int FillFrameBuffer() {
    t_ += a->FillFrameBuffer(t_ <= 0);
  }
  void Advance(int t) { t_ -= t; }
};
class Combine {
  DisplayHelper a_, b_;
  int last_t_;

  int FilleFrameBuffer(bool advance) {
    if (advance) {
      a_.Advance(last_t_);
      b_.Advance(last_t_);
    }
    int last_t_ = a_.FillFrameBuffer();
    if (last_t_ > 0) last_t_ = std::min(last_t_, b_.FillFrameBuffer());
    return last_t_;
  }
};
#endif

template<int Width, class col_t>
class StandardDisplayController : public DisplayControllerBase<Width, col_t>, SaberBase, private AudioStreamWork
#ifdef ENABLE_DEVELOPER_COMMANDS
  , CommandParser
#endif
{
public:
  static const int WIDTH = Width;
  static const int HEIGHT = sizeof(col_t) * 8;

  enum ScreenLayout {
    LAYOUT_NATIVE,
    LAYOUT_LANDSCAPE,
    LAYOUT_PORTRAIT,
  };

  enum class Tristate : uint8_t {
    Unknown,
    True,
    False
  };

  Display<Width, col_t>* display_;
  virtual void SetDisplay(Display<Width, col_t>* display) override {
    display_ = display;
  }

  // Clear and go to native mode.
  void Clear() {
    display_->Clear();
    layout_ = LAYOUT_NATIVE;
    xor_ = 0;
    invert_y_ = false;
  }

  // Idempotent
  void ConvertToNative() {
    bool invert_y = invert_y_;
    switch (layout_) {
    case LAYOUT_NATIVE:
      break;
    case LAYOUT_LANDSCAPE:
      display_->ConvertLandscape(invert_y);
      invert_y = false;
      break;
    case LAYOUT_PORTRAIT:
      display_->ConvertPortrait();
    }
    if (invert_y) {
      display_->FlipY();
    }
    if (xor_) {
      display_->Invert();
    }
  }

  virtual Screen GetScreen() { return screen_; }

  uint32_t last_delay_ = 0;
  uint32_t t_ = 0;  // time since we switched screen

  // Fill frame buffer and return how long to display it.
  int FillFrameBuffer(bool advance) override {
    if (advance) {
      if (next_screen_ != SCREEN_UNSET) {
        screen_ = next_screen_;
        next_screen_ = SCREEN_UNSET;
        t_ = 0;
      } else {
        t_ += last_delay_;
      }
    }
    return last_delay_ = FillFrameBuffer2(advance);
  }

#ifdef USB_CLASS_MSC
  bool EscapeIdleIfNeeded() {
    return looped_idle_ == Tristate::True && USBD_Configured();
  }
#else
  bool EscapeIdleIfNeeded() { return false; }
#endif

  void ShowDefault(bool ignore_lockup = false) {
    screen_ = SCREEN_PLI;
    t_ = 0;
    if (SaberBase::IsOn()) {
      if (SaberBase::Lockup() && IMG_lock && !ignore_lockup) {
        SetFile(&IMG_lock, 3600000.0);
      } else if (looped_on_ != Tristate::False) {
        SetFile(&IMG_on, font_config.ProffieOSOnImageDuration);
      }
    } else {
      // Off
      if (looped_idle_ != Tristate::False) {
        if (EscapeIdleIfNeeded()) {
          SetMessage("    usb\nconnected");
        } else {
          SetFile(&IMG_idle, 3600000.0);
        }
      }
    }
  }

  int FillFrameBuffer2(bool advance) {
    switch (screen_) {
      default:
      case SCREEN_UNSET:
      case SCREEN_OFF:
        Clear();
        return 3600000; // Long time!

      case SCREEN_STARTUP:
        Clear();
        // DrawText("==SabeR===", 0,15, Starjedi10pt7bGlyphs);
        // DrawText("++Teensy++",-4,31, Starjedi10pt7bGlyphs);
        if (WIDTH < 128) {
          display_->DrawText("p-os", 0,15, Starjedi10pt7bGlyphs);
        } else {
          display_->DrawText("proffieos", 0,15, Starjedi10pt7bGlyphs);
        }
        display_->DrawText(version,0,31, Starjedi10pt7bGlyphs);
        if (HEIGHT > 32) {
          display_->DrawText("installed: ",0,47, Starjedi10pt7bGlyphs);
          display_->DrawText(install_time,0,63, Starjedi10pt7bGlyphs);
        }
        next_screen_ = SCREEN_PLI;
        if (font_config.ProffieOSTextMessageDuration != -1) {
          return font_config.ProffieOSTextMessageDuration;
        } else if (font_config.ProffieOSFontImageDuration > 0) {
          return font_config.ProffieOSFontImageDuration;
        } else {
          return 3500;
        }

      case SCREEN_PLI:
        if (!SaberBase::IsOn() && t_ >= PLI_OFF_TIME) {
          STDERR << "Time = " << t_ << ". PLI_OFF_TIME expired.\n";
          screen_ = SCREEN_OFF;
          return FillFrameBuffer2(advance);
        }
        Clear();
        display_->DrawBatteryBar(BatteryBar16, battery_monitor.battery_percent());
        if (HEIGHT > 32) {
          char tmp[32];
          strcpy(tmp, "volts x.xx");
          float v = battery_monitor.battery();
          tmp[6] = '0' + (int)floorf(v);
          tmp[8] = '0' + ((int)floorf(v * 10)) % 10;
          tmp[9] = '0' + ((int)floorf(v * 100)) % 10;
          display_->DrawText(tmp,0,55, Starjedi10pt7bGlyphs);
        }
        return 200;  // redraw once every 200 ms

      case SCREEN_ERROR_MESSAGE:
      case SCREEN_MESSAGE: {
	uint32_t t;
        if (font_config.ProffieOSTextMessageDuration != -1) {
	  t = font_config.ProffieOSTextMessageDuration;
        } else if (font_config.ProffieOSFontImageDuration > 0) {
	  t = font_config.ProffieOSFontImageDuration;
        } else {
	  t = 3500;
        }
	if (t_ >= t) {
	  screen_ = SCREEN_DEFAULT;
	  ShowDefault();
	  return FillFrameBuffer2(advance);
	}
        Clear();
        // Aurebesh Font option.
#ifdef USE_AUREBESH_FONT
        const Glyph* font = Aurebesh10pt7bGlyphs;
#else
        const Glyph* font = Starjedi10pt7bGlyphs;
#endif
        if (strchr(message_, '\n')) {
          display_->DrawText(message_, 0, 15, font);
        } else {
        // centered
          display_->DrawText(message_, 0, HEIGHT / 2 + 7, font);
        }
        return 200;  // redraw once every 200 ms
      }

      case SCREEN_IMAGE:
        if (EscapeIdleIfNeeded() && current_effect_ == &IMG_idle) {
          // We are idle-looping, and usb is connected. Time to stop.
          SetMessage("    usb\nconnected");
          return FillFrameBuffer2(advance);
        }
        MountSDCard();
        {
          int count = 0;
          while (!frame_available_) {
            if (count++ > 3) return 0;
            if (eof_) advance = false;
            advance_ = advance;
            lock_fb_ = false;
            scheduleFillBuffer();
          }
        }
        lock_fb_ = true;
        if (eof_) {
          // STDERR << "FRAME COUNT @ EOF= " << frame_count_ << " left=" << (effect_display_duration_ - t_) <<  "\n";
          if (frame_count_ == 1 && t_ < effect_display_duration_) {
            ConvertToNative();
            frame_available_ = false;
            return effect_display_duration_ - t_;
          }
        } else {
          if (frame_available_ && advance) frame_count_++;
          if (looped_frames_ == 1 || t_ < effect_display_duration_) {
            ConvertToNative();
            frame_available_ = false;
            if (font_config.ProffieOSAnimationFrameRate > 0.0) {
              return 1000 / font_config.ProffieOSAnimationFrameRate;
            }
            if (looped_frames_ > 1) {
              return 1000 / looped_frames_;
            } else {
              // STDERR << "-> 41\n";
              return 41;   // ~24 fps
            }
          }
        }

        // This image/animation is done, time to choose the next thing to display.
      case SCREEN_DEFAULT:
        // STDERR << "MOVING ON...\n";
        ShowDefault();
        return FillFrameBuffer2(advance);
    }
  }

  void SB_On() override {
    if (!ShowFile(&IMG_on, font_config.ProffieOSOnImageDuration)) {
      ShowDefault();
      last_delay_ = t_ = 0;
      display_->Page();
    }
  }

  void SB_On2() override {
    if (IMG_out) {
      ShowFileWithSoundLength(&IMG_out, font_config.ProffieOSOutImageDuration);
    }
  }

  void usb_connected() override {
    if (EscapeIdleIfNeeded() && current_effect_ == &IMG_idle) {
      // We are idle-looping, and usb is connected. Time to stop.
      SetMessage("    usb\nconnected");
      SetScreenNow(SCREEN_MESSAGE);
    }
  }

  void ShowFileWithSoundLength(Effect* e, float duration) {
    if (duration == 0.0) {
      duration = SaberBase::sound_length * 1000;
    }
    ShowFile(e, round(duration));
  }

 void SB_Effect2(EffectType effect, float location) override {
   switch (effect) {
     case EFFECT_NEWFONT:
       looped_on_ = Tristate::Unknown;
       looped_idle_ = Tristate::Unknown;
       if (IMG_font) {
	 ShowFileWithSoundLength(&IMG_font, font_config.ProffieOSFontImageDuration);
       } else if (prop.current_preset_name()) {
	 SetMessage(prop.current_preset_name());
	 SetScreenNow(SCREEN_MESSAGE);
       } else if (IMG_idle) {
	 ShowFile(&IMG_idle, 3600000.0);
       }
       break;
     case EFFECT_LOCKUP_BEGIN:
       ShowDefault();
       break;
     case EFFECT_LOCKUP_END:
       ShowDefault(true);
       break;
     case EFFECT_BATTERY_LEVEL:
       // Show On-Demand battery meter
       SetScreenNow(SCREEN_PLI);
       break;
     case EFFECT_SD_CARD_NOT_FOUND:
       SetErrorMessage("sd card\nnot found");
       break;
     case EFFECT_ERROR_IN_FONT_DIRECTORY:
       SetErrorMessage("err font\ndirectory");
       break;
     case EFFECT_ERROR_IN_BLADE_ARRAY:
       SetErrorMessage("err blade\narray");
       break;
     case EFFECT_FONT_DIRECTORY_NOT_FOUND:
       SetErrorMessage("font dir\nnot found");
       break;
     case EFFECT_LOW_BATTERY:
       // Maybe we should make this blink or something?
       if (IMG_lowbatt) {
	 ShowFile(&IMG_lowbatt, 5000);
       } else {
	 SetErrorMessage("low\nbattery");
       }
       break;
     case EFFECT_BOOT:
       if (IMG_boot) {
	 ShowFileWithSoundLength(&IMG_boot,
				 font_config.ProffieOSBootImageDuration != -1.0 ?
				 font_config.ProffieOSBootImageDuration :
				 font_config.ProffieOSFontImageDuration);
       } else {
	 SetScreenNow(SCREEN_STARTUP);
       }
       break;
     case EFFECT_BLAST:
       ShowFileWithSoundLength(&IMG_blst, font_config.ProffieOSBlastImageDuration);
       break;
     case EFFECT_CLASH:
       ShowFileWithSoundLength(&IMG_clsh, font_config.ProffieOSClashImageDuration);
       break;
     case EFFECT_FORCE:
       ShowFileWithSoundLength(&IMG_force, font_config.ProffieOSForceImageDuration);
       break;
     case EFFECT_PREON:
       ShowFile(&IMG_preon, round(SaberBase::sound_length * 1000));
       break;
     case EFFECT_POSTOFF:
       ShowFileWithSoundLength(&IMG_pstoff, font_config.ProffieOSPstoffImageDuration);
       break;
/* To-Do, possibly differently
   #ifdef OLED_USE_BLASTER_IMAGES
   case EFFECT_RELOAD:
   ShowFileWithSoundLength(&IMG_reload, font_config.ProffieOSReloadImageDuration);
   break;
   case EFFECT_EMPTY:
   ShowFileWithSoundLength(&IMG_empty, font_config.ProffieOSEmptyImageDuration);
   break;
   case EFFECT_JAM:
   ShowFileWithSoundLength(&IMG_jam, font_config.ProffieOSJamImageDuration);
   break;
   case EFFECT_CLIP_IN:
   ShowFileWithSoundLength(&IMG_clipin, font_config.ProffieOSClipinImageDuration);
   break;
   case EFFECT_CLIP_OUT:
   ShowFileWithSoundLength(&IMG_clipout, font_config.ProffieOSClipoutImageDuration);
   break;
   case EFFECT_DESTRUCT:
   ShowFileWithSoundLength(&IMG_destruct, font_config.ProffieOSDestructImageDuration);
   break;
   #endif
*/
       break;
     default: break;
   }
 }

  // If called from SB_Effect2, you must call SetScreenNow after.
  void SetMessage(const char* text) {
    if (screen_ == SCREEN_ERROR_MESSAGE) return;
    strncpy(message_, text, sizeof(message_));
    message_[sizeof(message_)-1] = 0;
    screen_ = SCREEN_MESSAGE;
  }

  // Calls SetScreenNow already.
  void SetErrorMessage(const char* text) {
    strncpy(message_, text, sizeof(message_));
    message_[sizeof(message_)-1] = 0;
    SetScreenNow(SCREEN_ERROR_MESSAGE);
  }

  void SB_Top(uint64_t total_cycles) override {
    display_->SB_Top();
  }

  void SB_Off2(OffType offtype) override {
    if (offtype == OFF_IDLE) {
      SetScreenNow(SCREEN_OFF);
    } else if (IMG_in) {
      ShowFileWithSoundLength(&IMG_in, font_config.ProffieOSInImageDuration);
    } else if (IMG_idle) {
      ShowFile(&IMG_idle, 3600000.0);
    } else {
      SetScreenNow(SCREEN_PLI);
    }
  }

  void SB_IsOn(bool* on) override {
    display_->SB_IsOn(on);
  }

  void SetScreenNow(Screen screen) {
    if (screen_ == SCREEN_ERROR_MESSAGE) return;
    // No need to wake the sleeping bear just to tell it to go to bed again.
    if (screen == SCREEN_OFF && screen_ == SCREEN_OFF) return;
    last_delay_ = t_ = 0;
    next_screen_ = screen_ = screen;
    display_->Page();
  }

  bool SetFile(Effect* effect, float duration) {
    if (screen_ == SCREEN_ERROR_MESSAGE) return false;
    if (!*effect) return false;
    MountSDCard();
    eof_ = true;
    file_.Play(effect);
    frame_available_ = false;
    frame_count_ = 0;
    screen_ = SCREEN_IMAGE;
    eof_ = false;
    current_effect_ = effect;
    effect_display_duration_ = duration;
    return true;
  }

  bool ShowFile(Effect* effect, float duration) {
    if (SetFile(effect, duration)) {
      SetScreenNow(SCREEN_IMAGE);
      return true;
    }
    return false;
  }

  void ShowFile(const char* file) {
    if (screen_ == SCREEN_ERROR_MESSAGE) return;
    MountSDCard();
    eof_ = true;
    file_.Play(file);
    frame_available_ = false;
    frame_count_ = 0;
    SetScreenNow(SCREEN_IMAGE);
    eof_ = false;
  }


  // AudioStreamWork implementation
  size_t space_available() const override {
    if (lock_fb_) return 0;
    if (eof_ && advance_) return 0;
    if (frame_available_) return 0;

    // Always low priority
    return 1;
  }

#define TAG2(X, Y) (((X) << 8) | (Y))
  int ReadColorAsGray(FileReader* f) {
    uint8_t rgba[4];
    f->Read(rgba, 4);
    return rgba[0] + rgba[1] + rgba[2];
  }

  bool ReadImage(FileReader* f) {
    uint32_t file_end = 0;
    // STDERR << "ReadImage " << f->Tell() << " size = " << f->FileSize() << "\n";
    if (ypos_ >= looped_frames_ || ypos_ == 0) {
      if (looped_frames_ > 1) f->Seek(0);
      ypos_ = 0;
      uint32_t file_start = f->Tell();
      int a = f->Read();
      int b = f->Read();
      int width, height;
      switch (TAG2(a, b)) {
        default:
          STDERR << "Unknown image format. a=" << a << " b=" << b << " pos=" << f->Tell() << "\n";
          return false;

        case TAG2('P', '4'):
          // PBM
          f->skipwhite();
          width = f->readIntValue();
          f->skipwhite();
          height = f->readIntValue();
          f->Read();
          xor_ = 255;
          invert_y_ = false;
          break;

        case TAG2('B', 'M'):
        case TAG2('B', 'A'):
        case TAG2('C', 'I'):
        case TAG2('C', 'P'):
        case TAG2('I', 'C'):
        case TAG2('P', 'T'):
          // STDERR << "BMP detected!\n";
          xor_ = 255;
          invert_y_ = true; // bmp data is bottom to top
          // BMP
          file_end = file_start + f->ReadType<uint32_t>();
          f->Skip(4);
          uint32_t offset = f->ReadType<uint32_t>();

#if 0
          uint32_t ctable = f->Tell() + f->ReadType<uint32_t>();
          // STDERR << "OFFSET = " << offset << " CTABLE=" << ctable << "\n";
          width = f->ReadType<uint32_t>();
          height = f->ReadType<uint32_t>();
          // STDERR << "Width=" << width << " Height=" << height << "\n";
          f->Seek(ctable);
          int c0 = ReadColorAsGray(f);
          int c1 = ReadColorAsGray(f);
          xor_ = c0 > c1 ? 255 : 0;
#else
          f->Skip(4);
          width = f->ReadType<uint32_t>();
          height = f->ReadType<uint32_t>();
          // STDERR << "Width=" << width << " Height=" << height << "\n";
#endif
          // First frame is near the end, seek to it.
          f->Seek(file_start + offset + width * height / 8 - sizeof(MonoFrame<WIDTH, col_t>::frame_buffer_));
      }
      if (width != WIDTH && width != HEIGHT) {
        STDERR << "Wrong size image: " << width << "x" << height << "\n";
        return false;
      }
      if (width == WIDTH) {
        layout_ = LAYOUT_LANDSCAPE;
        looped_frames_ = height / HEIGHT;
      } else {
        looped_frames_ = height / WIDTH;
      }
      if (current_effect_ == &IMG_on) {
        looped_on_ = looped_frames_ > 1 ? Tristate::True : Tristate::False;
      }
      if (current_effect_ == &IMG_idle) {
        looped_idle_ = looped_frames_ > 1 ? Tristate::True : Tristate::False;
      }
    }
    // STDERR << "ypos=" << ypos_ << " avail=" << f->Available() << "\n";
    if (f->Available() < sizeof(MonoFrame<WIDTH, col_t>::frame_buffer_)) return false;
    f->Read((uint8_t*)display_->frame_buffer_, sizeof(MonoFrame<WIDTH, col_t>::frame_buffer_));
    ypos_++;
    if (looped_frames_ > 1) {
      if (ypos_ >= looped_frames_) {
        f->Seek(f->FileSize());
      } else {
        if (invert_y_) {
          // Seek two frames back, because BMP are backwards.
          f->Seek(f->Tell() - 2 * sizeof(MonoFrame<WIDTH, col_t>::frame_buffer_));
        }
      }
    } else {
      if (file_end) f->Seek(file_end);
    }
    return true;
  }

  struct ReadState {
    int ypos = 0;
    uint32_t file_pos = 0;
  };

  bool FillBuffer() override {
    if (eof_ && advance_) {
      // STDERR << "e&a\n";
      return true;
    }
    if (file_.OpenFile()) {
      if (!file_.IsOpen()) {
        eof_ = true;
  // STDERR << "not open\n";
        return false;
      }
      ypos_ = looped_frames_;
      return true;  // return to ProcessAudioStream which will return here when no audiostreams are in need of data.
    }
    if (lock_fb_) {
      // STDERR << "locked\n";
      return true;
    }
    if (!frame_available_) {
      // STDERR << "ADVANCE=" << advance_ << " last_file_pos_= " << last_state_.file_pos << " ypos=" << last_state_.ypos << "\n";
      if (!advance_) {
        file_.Seek(last_state_.file_pos);
        ypos_ = last_state_.ypos;
      } else {
        advance_ = false;
      }
      ReadState state;
      state.ypos = ypos_;
      state.file_pos = file_.Tell();
      if (ReadImage(&file_)) {
        frame_available_ = true;
        last_state_ = state;
      } else {
        STDERR << "read image fail\n";
        eof_ = true;
      }
    }
    return true;
  }

  bool IsActive() override {
    return screen_ == SCREEN_IMAGE;
  }

  void CloseFiles() override {
    file_.Close();
  }

#ifdef ENABLE_DEVELOPER_COMMANDS
  bool Parse(const char* cmd, const char* e) override {
    if (!strcmp(cmd, "setmessage") && e) {
      STDOUT << "Setting message: " << e << "\n";
      SetMessage(e);
      SetScreenNow(SCREEN_MESSAGE);
      return true;
    }
    return false;
  }
#endif

private:
  // Variables related to frame buffer layout.
  uint8_t xor_ = 0;
  bool invert_y_ = 0;
  volatile ScreenLayout layout_;

  // Screen state
  Screen screen_ = SCREEN_STARTUP;
  Screen next_screen_ = SCREEN_UNSET;
  char message_[32];

  // File reading
  EffectFileReader file_;
  volatile bool frame_available_ = true;
  volatile bool eof_ = true;
  int32_t frame_count_ = 0;
  volatile int32_t looped_frames_ = 0;
  int32_t ypos_ = 0;
  bool lock_fb_ = false;
  ReadState last_state_;
  volatile bool advance_ = true;

  // True if IMG_on is looped.
  volatile Tristate looped_on_ = Tristate::Unknown;
  // True if IMG_idle is looped.
  volatile Tristate looped_idle_ = Tristate::Unknown;
  volatile float effect_display_duration_;
  volatile Effect* current_effect_;
};

template<template<int, class> class T>
struct BaseLayerOp {
  template<int Width, class col_t> struct Controller : public T<Width, col_t> {};
};

template<int WIDTH, class col_t, class POWER_PIN = PowerPINS<> >
class SSD1306Template : public Display<WIDTH, col_t>, I2CDevice, Looper, StateMachine {
public:
  static const int HEIGHT = sizeof(col_t) * 8;
  const char* name() override { return "SSD1306"; }

  enum Commands {
    SETCONTRAST = 0x81,
    DISPLAYALLON_RESUME = 0xA4,
    DISPLAYALLON = 0xA5,
    NORMALDISPLAY = 0xA6,
    INVERTDISPLAY = 0xA7,
    DISPLAYOFF = 0xAE,
    DISPLAYON = 0xAF,

    SETDISPLAYOFFSET = 0xD3,
    SETCOMPINS = 0xDA,

    SETVCOMDETECT = 0xDB,

    SETDISPLAYCLOCKDIV = 0xD5,
    SETPRECHARGE = 0xD9,

    SETMULTIPLEX = 0xA8,

    SETLOWCOLUMN = 0x00,
    SETHIGHCOLUMN = 0x10,

    SETSTARTLINE = 0x40,

    MEMORYMODE = 0x20,
    COLUMNADDR = 0x21,
    PAGEADDR   = 0x22,

    COMSCANINC = 0xC0,
    COMSCANDEC = 0xC8,

    SEGREMAP = 0xA0,

    CHARGEPUMP = 0x8D,

    EXTERNALVCC = 0x1,
    SWITCHCAPVCC = 0x2,

    // Scrolling commands
    ACTIVATE_SCROLL = 0x2F,
    DEACTIVATE_SCROLL = 0x2E,
    SET_VERTICAL_SCROLL_AREA = 0xA3,
    RIGHT_HORIZONTAL_SCROLL = 0x26,
    LEFT_HORIZONTAL_SCROLL = 0x27,
    VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29,
    VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 0x2A,
  };

  void SetController(DisplayControllerBase<WIDTH, col_t>* controller) {
    controller_ = controller;
    controller->SetDisplay(this);
  }

  explicit SSD1306Template(DisplayControllerBase<WIDTH, col_t>* controller) : I2CDevice(0x3C) {
    SetController(controller);
  }
  explicit SSD1306Template(DisplayControllerBase<WIDTH, col_t>* controller, int id) : I2CDevice(id) {
    SetController(controller);
  }
  void Send(int c) { writeByte(0, c); }

  static const size_t chunk_size = WIDTH * HEIGHT / 8 / 16;
  static const size_t num_chunks = WIDTH * HEIGHT / 8 / chunk_size;

  uint8_t chunk[chunk_size + 1];
  void GetChunk() {
    chunk[0] = 0x40;
    memcpy(chunk + 1, i + (unsigned char *)Display<WIDTH, col_t>::frame_buffer_, chunk_size);
  }

  int FillFrameBuffer() {
    return controller_->FillFrameBuffer(true);
  }

  void Page() override {
    millis_to_display_ = 0;
    next_millis_to_display_ = 0;
  }
  void SB_Top() override {
    STDOUT.print("display fps: ");
    loop_counter_.Print();
    STDOUT.println("");
  }

  Screen GetScreen() override {
    return controller_->GetScreen();
  }

  // TODO: Don't update the display when we don't need to
  // and return false here so that we can go into lower power modes.
  void SB_IsOn(bool* on) override {
    *on = on_;
  }

  void Loop() override {
#ifdef USB_CLASS_MSC
    static bool last_connected = false;
    bool connected = USBD_Configured();
    if (connected && !last_connected) controller_->usb_connected();
    last_connected = connected;
#endif

    STATE_MACHINE_BEGIN();
    while(true) {
      on_ = true;
      power_.Init();
      power_.Power(true);

      while (!i2cbus.inited()) YIELD();
      while (!I2CLock()) YIELD();

      // Init sequence
      Send(DISPLAYOFF);                    // 0xAE
      Send(SETDISPLAYCLOCKDIV);            // 0xD5
      Send(0x80);                          // the suggested ratio 0x80

      Send(SETMULTIPLEX);                  // 0xA8
      Send(HEIGHT - 1);

      Send(SETDISPLAYOFFSET);              // 0xD3
      Send(0x0);                                   // no offset

      Send(SETSTARTLINE | 0x0);            // 0x40 line #0

      Send(CHARGEPUMP);                    // 0x8D
      Send(0x14);

      Send(MEMORYMODE);                    // 0x20
      Send(0x01);                          // vertical address mode

#if defined (OLED_FLIP_180)
#if defined (OLED_MIRRORED)
      // Flip 180 and mirrored OLED operation
      Send(SEGREMAP | 0x1);        // 0xa0 | 1
#else
      // Flip 180
      Send(SEGREMAP);        // 0xa0 | 1
#endif
      Send(COMSCANINC);
#elif defined (OLED_MIRRORED)
      // mirrored OLED operation
      Send(SEGREMAP);        // 0xa0 | 1
      Send(COMSCANDEC);
#else
      // normal OLED operation
      Send(SEGREMAP | 0x1);        // 0xa0 | 1
      Send(COMSCANDEC);
#endif


      Send(SETCOMPINS);                    // 0xDA
      if (HEIGHT == 64 || WIDTH==64) {
        Send(0x12);
      } else {
        Send(0x02);  // may need to be 0x12 for some displays
      }
      Send(SETCONTRAST);                   // 0x81
      Send(0x8F);
      Send(SETPRECHARGE);                  // 0xd9
      Send(0xF1);
      Send(SETVCOMDETECT);                 // 0xDB
      Send(0x40);
      Send(DISPLAYALLON_RESUME);           // 0xA4
      Send(NORMALDISPLAY);                 // 0xA6

      Send(DEACTIVATE_SCROLL);

      Send(DISPLAYON);                     //--turn on oled panel

      I2CUnlock();

      STDOUT.println("Display initialized.");

      while (true) {
        millis_to_display_ = next_millis_to_display_;
        next_millis_to_display_ = 0;
        while (millis_to_display_ == 0) {
          YIELD();
          millis_to_display_ = FillFrameBuffer();
          // STDERR << "millis_to_display_ = " << millis_to_display_ << "\n";
        }
        frame_start_time_ = millis();
        lock_fb_ = true;

        // STDOUT << "SCREEN = " << (int)GetScreen() << " m= " << millis_to_display_ << "  clear=" << Display<WIDTH, col_t>::isClear() << "\n";
        if (GetScreen() == SCREEN_OFF && Display<WIDTH, col_t>::isClear()) break;

        // I2C
        loop_counter_.Update();
#ifdef PROFFIEBOARD
        i = -(int)NELEM(transactions);
        while (!I2CLockAndRun()) YIELD();
        while (lock_fb_) YIELD();
#else
        do { YIELD(); } while (!I2CLock());
        Send(COLUMNADDR);
        Send((128 - WIDTH)/2);   // Column start address (0 = reset)
        Send(WIDTH-1 + (128 - WIDTH)/2); // Column end address (127 = reset)

        Send(PAGEADDR);
        Send(0); // Page start address (0 = reset)
        Send(sizeof(col_t) - 1);

        //STDOUT.println(TWSR & 0x3, DEC);

        for (i=0; i < WIDTH * HEIGHT / 8; ) {
          // send a bunch of data in one xmission
          Wire.beginTransmission(address_);
          GetChunk();
          for (size_t x=0; x <= chunk_size; x++) {
            Wire.write(chunk[x]);
          }
          Wire.endTransmission();
          I2CUnlock(); do { YIELD(); } while (!I2CLock());
        }
        lock_fb_ = false;
        I2CUnlock();
#endif
        while (millis() - frame_start_time_ < millis_to_display_) {
          if (next_millis_to_display_ == 0) {
            next_millis_to_display_ = FillFrameBuffer();
            // STDERR << "next_millis_to_display_ = " << next_millis_to_display_ << "\n";

          }
          YIELD();
        }
      }

      STDERR << "DISPLAY SLEEP\n";

      // Time to shut down... for now.
      while (!I2CLock()) YIELD();
      Send(DISPLAYOFF);                    // 0xAE
      I2CUnlock();

      power_.Power(false);
      power_.DeInit();
      on_ = false;
      while (GetScreen() == SCREEN_OFF) YIELD();
      STDERR << "DISPLAY WAKEUP\n";
    }

    STATE_MACHINE_END();
  }

#ifdef PROFFIEBOARD
  static constexpr uint8_t transactions[] = {
    COLUMNADDR,
    (128 - WIDTH)/2,   // Column start address (0 = reset)
    WIDTH-1 + (128 - WIDTH)/2, // Column end address (127 = reset)
    PAGEADDR,
    0,  // Page start address (0 = reset)
    sizeof(col_t) - 1
  };
  void RunLocked() override {
    size_t size;
    if (i < 0) {
      chunk[0] = 0;
      chunk[1] = transactions[NELEM(transactions)+i];
      i++;
      size = 2;
    } else {
      GetChunk();
      size = chunk_size + 1;
      i += chunk_size;
    }
    if (!stm32l4_i2c_notify(Wire._i2c, &SSD1306Template::DataSent, this, (I2C_EVENT_ADDRESS_NACK | I2C_EVENT_DATA_NACK | I2C_EVENT_ARBITRATION_LOST | I2C_EVENT_BUS_ERROR | I2C_EVENT_OVERRUN | I2C_EVENT_RECEIVE_DONE | I2C_EVENT_TRANSMIT_DONE | I2C_EVENT_TRANSFER_DONE))) {
      goto fail;
    }
    if (!stm32l4_i2c_transmit(Wire._i2c, address_, chunk, size, 0)) {
      goto fail;
    }
    return;
  fail:
    lock_fb_ = false;
    I2CUnlock();
    return;
  }
  static void DataSent(void *x, unsigned long event) { ((SSD1306Template*)x)->DataSent(); }
  void DataSent() {
    stm32l4_i2c_notify(Wire._i2c, nullptr, 0, 0);
    I2CUnlock();
    if (i < WIDTH * HEIGHT / 8) {
      I2CLockAndRun();
    } else {
      lock_fb_ = false;
    }
  }
#endif

private:
  int i;
  uint32_t millis_to_display_;
  uint32_t next_millis_to_display_;
  uint32_t frame_start_time_;
  volatile bool lock_fb_ = false;
  DisplayControllerBase<WIDTH, col_t>* controller_;
  LoopCounter loop_counter_;
  POWER_PIN power_;
  bool on_ = false;
};

#ifdef PROFFIEBOARD
template<int WIDTH, class col_t, class POWER_PIN>
constexpr uint8_t SSD1306Template<WIDTH, col_t, POWER_PIN>::transactions[];
#endif

using SSD1306 = SSD1306Template<128, uint32_t>;

#endif
