#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H

#include "monoframe.h"

// Images/animations
IMAGE_FILESET(font);
IMAGE_FILESET(boot);
IMAGE_FILESET(on);
IMAGE_FILESET(clsh);
IMAGE_FILESET(blst);
IMAGE_FILESET(lock);
IMAGE_FILESET(force);


template<int Width, class col_t>
class Display {
public:
  virtual void Page() = 0;
  virtual void SB_Top() = 0;
};

template<int Width, class col_t>
class DisplayControllerBase : public MonoFrame<Width, col_t> {
public:
  // Return zero if frame is not yet ready.
  // May be called before it's time to actually display next frame.
  virtual int FillFrameBuffer() = 0;
  virtual void SetDisplay(Display<Width, col_t>* display) = 0;
};

template<int Width, class col_t>
class StandardDisplayController : public DisplayControllerBase<Width, col_t>, SaberBase, private AudioStreamWork {
public:
  static const int WIDTH = Width;
  static const int HEIGHT = sizeof(col_t) * 8;
  enum Screen {
    SCREEN_STARTUP,
    SCREEN_MESSAGE,
    SCREEN_PLI,
    SCREEN_IMAGE,  // also for animations
    SCREEN_OFF,
  };

  enum ScreenLayout {
    LAYOUT_NATIVE,
    LAYOUT_LANDSCAPE,
    LAYOUT_PORTRAIT,
  };

  Display<Width, col_t>* display_;
  virtual void SetDisplay(Display<Width, col_t>* display) override {
    display_ = display;
  }

  // Clear and go to native mode.
  void Clear() {
    MonoFrame<WIDTH, col_t>::Clear();
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
      MonoFrame<WIDTH, col_t>::ConvertLandscape(invert_y);
      invert_y = false;
      break;
    case LAYOUT_PORTRAIT:
      MonoFrame<WIDTH, col_t>::ConvertPortrait();
    }
    if (invert_y) {
      MonoFrame<WIDTH, col_t>::FlipY();
    }
    if (xor_) {
      MonoFrame<WIDTH, col_t>::Invert();
    }
  }

  // Fill frame buffer and return how long to display it.
  int FillFrameBuffer() override {
    switch (screen_) {
      default:
      case SCREEN_OFF:
	Clear();
        return 1000;

      case SCREEN_STARTUP:
	Clear();
        // DrawText("==SabeR===", 0,15, Starjedi10pt7bGlyphs);
        // DrawText("++Teensy++",-4,31, Starjedi10pt7bGlyphs);
	if (WIDTH < 128) {
	  MonoFrame<WIDTH, col_t>::DrawText("p-os", 0,15, Starjedi10pt7bGlyphs);
	} else {
	  MonoFrame<WIDTH, col_t>::DrawText("proffieos", 0,15, Starjedi10pt7bGlyphs);
	}
	MonoFrame<WIDTH, col_t>::DrawText(version,0,31, Starjedi10pt7bGlyphs);
	if (HEIGHT > 32) {
	  MonoFrame<WIDTH, col_t>::DrawText("installed: ",0,47, Starjedi10pt7bGlyphs);
	  MonoFrame<WIDTH, col_t>::DrawText(install_time,0,63, Starjedi10pt7bGlyphs);
	}
        screen_ = SCREEN_PLI;
        return font_config.ProffieOSFontImageDuration;

      case SCREEN_PLI:
	Clear();
        MonoFrame<WIDTH, col_t>::DrawBatteryBar(BatteryBar16, battery_monitor.battery_percent());
	if (HEIGHT > 32) {
	  char tmp[32];
	  strcpy(tmp, "volts x.xx");
	  float v = battery_monitor.battery();
	  tmp[6] = '0' + (int)floorf(v);
	  tmp[8] = '0' + ((int)floorf(v * 10)) % 10;
	  tmp[9] = '0' + ((int)floorf(v * 100)) % 10;
	  MonoFrame<WIDTH, col_t>::DrawText(tmp,0,55, Starjedi10pt7bGlyphs);
	}
        return 200;  // redraw once every 200 ms

      case SCREEN_MESSAGE: {
	Clear();
     // Aurebesh Font option.
      #ifdef USE_AUREBESH_FONT
        const Glyph* font = Aurebesh10pt7bGlyphs;
      #else
        const Glyph* font = Starjedi10pt7bGlyphs;
      #endif
        if (strchr(message_, '\n')) {
          MonoFrame<WIDTH, col_t>::DrawText(message_, 0, 15, font);
        } else {
	  // centered
          MonoFrame<WIDTH, col_t>::DrawText(message_, 0, HEIGHT / 2 + 7, font);
        }
        screen_ = SCREEN_PLI;
        return font_config.ProffieOSFontImageDuration;
      }

      case SCREEN_IMAGE:
        MountSDCard();
        if (eof_) {
          // STDOUT << "EOF " << frame_count_ << "\n";
          if (!SaberBase::IsOn()) {
            screen_ = SCREEN_PLI;
            if (frame_count_ == 1) return font_config.ProffieOSFontImageDuration;
            return FillFrameBuffer();
          }
        }
        if (!frame_available_) {
	  lock_fb_ = false;
          scheduleFillBuffer();
	  if (!frame_available_) {
	    return 0;
	  }
        }
	lock_fb_ = true;
        frame_count_++;
        if (SaberBase::IsOn()) {
          // Single frame image
          if (looped_frames_ == 1) {
            if (frame_count_ == 1) {
	      ConvertToNative();
	      frame_available_ = false;
              return effect_display_duration_;
            }
            screen_ = SCREEN_PLI;
            if (SaberBase::Lockup()) {
              ShowFile(&IMG_lock, 3600000.0);
	      ConvertToNative();
              return 3600000;
            } else {
              if (looped_on_) ShowFile(&IMG_on, font_config.ProffieOSOnImageDuration);
            }
          } else {
            // looped image
            if (looped_frames_ == frame_count_) {
              if (millis() - loop_start_ > effect_display_duration_) {
                if (!SaberBase::Lockup()) {
                  screen_ = SCREEN_PLI;
                  if (looped_on_) ShowFile(&IMG_on, font_config.ProffieOSOnImageDuration);
                }
              }
            }
          }
        } else {
          if (millis() - loop_start_ > font_config.ProffieOSFontImageDuration) {
            STDOUT << "END OF LOOP\n";
            screen_ = SCREEN_PLI;
          }
        }
	ConvertToNative();
	frame_available_ = false;
        if (font_config.ProffieOSAnimationFrameRate > 0.0) {
          return 1000 / font_config.ProffieOSAnimationFrameRate;
        }
        if (looped_frames_ > 1) {
          return 1000 / looped_frames_;
        } else {
          return 41;   // ~24 fps
        }
    }
  }

  void SB_On() override {
    ShowFile(&IMG_on, font_config.ProffieOSOnImageDuration);
  }

  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
      case EFFECT_BOOT:
	if (IMG_boot) {
	  ShowFile(&IMG_boot, font_config.ProffieOSFontImageDuration);
	}
	return;
      case EFFECT_NEWFONT:
	ShowFile(&IMG_font, font_config.ProffieOSFontImageDuration);
	return;
      case EFFECT_BLAST:
	ShowFile(&IMG_blst, font_config.ProffieOSBlastImageDuration);
	return;
      case EFFECT_CLASH:
	ShowFile(&IMG_clsh, font_config.ProffieOSClashImageDuration);
	return;
      case EFFECT_FORCE:
	ShowFile(&IMG_force, font_config.ProffieOSForceImageDuration);
	return;
      case EFFECT_LOCKUP_BEGIN:
	ShowFile(&IMG_lock, 3600000.0);
	return;
      case EFFECT_LOCKUP_END:
	screen_ = SCREEN_PLI;
	if (looped_on_) ShowFile(&IMG_on, font_config.ProffieOSOnImageDuration);
	return;

      default: break;
    }
  }

  void SB_Message(const char* text) override {
    strncpy(message_, text, sizeof(message_));
    message_[sizeof(message_)-1] = 0;
    SetScreenNow(SCREEN_MESSAGE);
  }

  void SB_Top(uint64_t total_cycles) override {
    display_->SB_Top();
  }

  void SB_Off(OffType offtype) override {
    // TODO: Make it so that screen can be
    // powered down and up again properly.
    // This only makes it black, which prevents burn-in.
    if (offtype == OFF_IDLE) {
      SetScreenNow(SCREEN_OFF);
    } else {
      SetScreenNow(SCREEN_PLI);
    }
  }

  // TODO: Don't update the display when we don't need to
  // and return false here so that we can go into lower power modes.
  void SB_IsOn(bool* on) override {
    *on = true;
  }

  void SetScreenNow(Screen screen) {
    screen_ = screen;
    display_->Page();
  }

  void ShowFile(Effect* effect, float duration) {
    if (*effect) {
      MountSDCard();
      eof_ = true;
      file_.Play(effect);
      frame_available_ = false;
      loop_start_ = millis();
      frame_count_ = 0;
      SetScreenNow(SCREEN_IMAGE);
      eof_ = false;
      current_effect_ = effect;
      effect_display_duration_ = duration;
    }
  }

  void ShowFile(const char* file) {
    MountSDCard();
    eof_ = true;
    file_.Play(file);
    frame_available_ = false;
    loop_start_ = millis();
    frame_count_ = 0;
    SetScreenNow(SCREEN_IMAGE);
    eof_ = false;
  }


  // AudioStreamWork implementation
  size_t space_available() const override {
    if (eof_ || lock_fb_) return 0;
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
    // STDOUT << "ReadImage " << f->Tell() << " size = " << f->FileSize() << "\n";
    if (ypos_ >= looped_frames_) {
      if (looped_frames_ > 1) f->Seek(0);
      ypos_ = 0;
      uint32_t file_start = f->Tell();
      int a = f->Read();
      int b = f->Read();
      int width, height;
      switch (TAG2(a, b)) {
        default:
          STDOUT << "Unknown image format. a=" << a << " b=" << b << " pos=" << f->Tell() << "\n";
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
          // STDOUT << "BMP detected!\n";
          xor_ = 255;
          invert_y_ = true; // bmp data is bottom to top
          // BMP
          file_end = file_start + f->ReadType<uint32_t>();
          f->Skip(4);
          uint32_t offset = f->ReadType<uint32_t>();

#if 0
          uint32_t ctable = f->Tell() + f->ReadType<uint32_t>();
          // STDOUT << "OFFSET = " << offset << " CTABLE=" << ctable << "\n";
          width = f->ReadType<uint32_t>();
          height = f->ReadType<uint32_t>();
          // STDOUT << "Width=" << width << " Height=" << height << "\n";
          f->Seek(ctable);
          int c0 = ReadColorAsGray(f);
          int c1 = ReadColorAsGray(f);
          xor_ = c0 > c1 ? 255 : 0;
#else
          f->Skip(4);
          width = f->ReadType<uint32_t>();
          height = f->ReadType<uint32_t>();
          // STDOUT << "Width=" << width << " Height=" << height << "\n";
#endif
          // First frame is near the end, seek to it.
          f->Seek(file_start + offset + width * height / 8 - sizeof(MonoFrame<WIDTH, col_t>::frame_buffer_));
      }
      if (width != WIDTH && width != HEIGHT) {
        STDOUT << "Wrong size image: " << width << "x" << height << "\n";
        return false;
      }
      if (width == WIDTH) {
        layout_ = LAYOUT_LANDSCAPE;
        looped_frames_ = height / HEIGHT;
      } else {
        looped_frames_ = height / WIDTH;
      }
      if (current_effect_ == &IMG_on) {
        looped_on_ = looped_frames_ > 1;
      }
    }
    // STDOUT << "ypos=" << ypos_ << " avail=" << f->Available() << "\n";
    if (f->Available() < sizeof(MonoFrame<WIDTH, col_t>::frame_buffer_)) return false;
    f->Read((uint8_t*)MonoFrame<WIDTH, col_t>::frame_buffer_, sizeof(MonoFrame<WIDTH, col_t>::frame_buffer_));
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

  bool FillBuffer() override {
    if (eof_) return true;
    if (file_.OpenFile()) {
      if (!file_.IsOpen()) {
        eof_ = true;
        return false;
      }
      ypos_ = looped_frames_;
    }
    if (lock_fb_) return true;
    if (!frame_available_) {
      if (!ReadImage(&file_)) {
        file_.Close();
        eof_ = true;
      }
      frame_available_ = true;
    }
    return true;
  }
  void CloseFiles() override {
    file_.Close();
  }

private:
  // Variables related to frame buffer layout.
  uint8_t xor_ = 0;
  bool invert_y_ = 0;
  volatile ScreenLayout layout_;

  // Screen state
  Screen screen_ = SCREEN_STARTUP;
  char message_[32];

  // File reading
  EffectFileReader file_;
  volatile bool frame_available_ = true;
  volatile bool eof_ = true;
  int32_t frame_count_ = 0;
  volatile int32_t looped_frames_ = 0;
  int32_t ypos_ = 0;
  bool lock_fb_ = false;

  // True if IMG_on is looped.
  volatile bool looped_on_ = false;
  volatile float effect_display_duration_;
  uint32_t loop_start_;
  volatile Effect* current_effect_;
};

template<int WIDTH, class col_t>
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
    memcpy(chunk + 1, i + (unsigned char *)controller_->frame_buffer_, chunk_size);
  }

  int FillFrameBuffer() {
    return controller_->FillFrameBuffer();
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
							 
  void Loop() override {
    STATE_MACHINE_BEGIN();
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

#ifndef OLED_FLIP_180
    // normal OLED operation
    Send(SEGREMAP | 0x1);        // 0xa0 | 1
    Send(COMSCANDEC);
#else
    // allows for 180deg rotation of the OLED mapping
    Send(SEGREMAP);
    Send(COMSCANINC);
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
      }
      frame_start_time_ = millis();
      lock_fb_ = true;

      // I2C
      loop_counter_.Update();
#ifdef PROFFIEBOARD
      i = -(int)NELEM(transactions);
      I2CLockAndRun();
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
	}
	YIELD();
      }
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
};

#ifdef PROFFIEBOARD
template<int WIDTH, class col_t>
constexpr uint8_t SSD1306Template<WIDTH, col_t>::transactions[];
#endif

using SSD1306 = SSD1306Template<128, uint32_t>;

#endif
