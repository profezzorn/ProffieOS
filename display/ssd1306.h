#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H

// Images/animations
IMAGE_FILESET(font);
IMAGE_FILESET(boot);
IMAGE_FILESET(on);
IMAGE_FILESET(clsh);
IMAGE_FILESET(blst);
IMAGE_FILESET(lock);
IMAGE_FILESET(force);

struct Glyph {
  int32_t skip : 7;
  int32_t xoffset : 7;
  int32_t yoffset : 7;
  int32_t columns : 8;
  int32_t column_size : 3;
  const void* data;
};

const uint32_t BatteryBar16_data[] = {
   0b00000000000000000000000000000000UL,
   0b00000111111111111111111111100000UL,
   0b00011111111111111111111111111000UL,
   0b00111111111111111111111111111100UL,
   0b01111111111111111111111111111110UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b11111111111111111111111111111111UL,
   0b01111111111111111111111111111110UL,
   0b00111111111111111111111111111100UL,
   0b00011111111111111111111111111000UL,
   0b00000111111111111111111111100000UL,
};

#define GLYPHDATA(X) NELEM(X), sizeof((X)[0]) - 1, (const void*)X

const Glyph BatteryBar16 = { 16, 0, 0, GLYPHDATA(BatteryBar16_data) };

#include "StarJedi10Font.h"

class SSD1306 : public I2CDevice, Looper, StateMachine, SaberBase, private AudioStreamWork {
public:
  static const int WIDTH = 128;
  static const int HEIGHT = 32;
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

  SSD1306() : I2CDevice(0x3C) { }
  void Send(int c) { writeByte(0, c); }

  template<typename T>
  void Draw2(int begin, int end, uint32_t* pos, int shift, const T* data) {
    if (shift > 0) {
      for (int i = begin; i < end; i++) pos[i] |= data[i] << shift;
    } else if (shift < 0) {
      for (int i = begin; i < end; i++) pos[i] |= data[i] >> -shift;
    } else {
      for (int i = begin; i < end; i++) pos[i] |= data[i];
    }
  }

  void Draw(const Glyph& glyph, int x, int y) {
    x += glyph.xoffset;
    y += glyph.yoffset;
    int begin = std::max<int>(0, -x);
    int end = std::min<int>(glyph.columns, WIDTH - x);
    uint32_t *pos = frame_buffer_ + x;
    switch (glyph.column_size) {
      case 0:
        Draw2<uint8_t>(begin, end, pos, y, (const uint8_t*)glyph.data);
        break;
      case 1:
        Draw2<uint16_t>(begin, end, pos, y, (const uint16_t*)glyph.data);
        break;
      case 3:
        Draw2<uint32_t>(begin, end, pos, y, (const uint32_t*)glyph.data);
        break;
    }
  }

  void DrawBatteryBar(const Glyph& bar) {
    int start, end;
    if (bar.skip < bar.columns) {
      start = -bar.skip;
      end = WIDTH + bar.columns - 1;
    } else {
      start = 0;
      end = WIDTH;
    }
    int max_bars = (end - start) / bar.skip;
    int pos = start;
    int bars = floorf(
        battery_monitor.battery_percent() * (0.5 + max_bars) / 100);
    for (int i = 0; i < bars; i++) {
      Draw(bar, pos, 0);
      pos += bar.skip;
    }
  }
  void DrawText(const char* str,
                int x, int y,
                const Glyph* font) {
    while (*str) {
      if (*str == '\n') {
        x = 0;
        y += 16;
      } else if (*str >= 0x20 && *str <= 0x7f) {
        Draw(font[*str - 0x20], x, y);
        x += font[*str - 0x20].skip;
      }
      str++;
    }
  }


  // Fill frame buffer and return how long to display it.
  int FillFrameBuffer() {
    switch (screen_) {
      default:
      case SCREEN_OFF:
        memset(frame_buffer_, 0, sizeof(frame_buffer_));
        layout_ = LAYOUT_NATIVE;
        xor_ = 0;
        invert_y_ = false;
        return 1000;

      case SCREEN_STARTUP:
        memset(frame_buffer_, 0, sizeof(frame_buffer_));
        // DrawText("==SabeR===", 0,15, Starjedi10pt7bGlyphs);
        // DrawText("++Teensy++",-4,31, Starjedi10pt7bGlyphs);
        DrawText("proffieos", 0,15, Starjedi10pt7bGlyphs);
        DrawText(version,0,31, Starjedi10pt7bGlyphs);
        screen_ = SCREEN_PLI;
        layout_ = LAYOUT_NATIVE;
        xor_ = 0;
        invert_y_ = false;
        return font_config.ProffieOSFontImageDuration;

      case SCREEN_PLI:
        memset(frame_buffer_, 0, sizeof(frame_buffer_));
        DrawBatteryBar(BatteryBar16);
        layout_ = LAYOUT_NATIVE;
        xor_ = 0;
        invert_y_ = false;
        return 200;  // redraw once every 200 ms

      case SCREEN_MESSAGE:
        memset(frame_buffer_, 0, sizeof(frame_buffer_));
        if (strchr(message_, '\n')) {
          DrawText(message_, 0, 15, Starjedi10pt7bGlyphs);
        } else {
          DrawText(message_, 0, 23, Starjedi10pt7bGlyphs);
        }
        screen_ = SCREEN_PLI;
        layout_ = LAYOUT_NATIVE;
        xor_ = 0;
        invert_y_ = false;
        return font_config.ProffieOSFontImageDuration;

      case SCREEN_IMAGE:
        MountSDCard();
        if (!frame_available_) {
          scheduleFillBuffer();
          return 1;
        }
        if (eof_) {
          // STDOUT << "EOF " << frame_count_ << "\n";
          if (!SaberBase::IsOn()) {
            screen_ = SCREEN_PLI;
            if (frame_count_ == 1) return font_config.ProffieOSFontImageDuration;
            return FillFrameBuffer();
          }
        }
        frame_count_++;
        if (SaberBase::IsOn()) {
          // Single frame image
          if (looped_frames_ == 1) {
            if (frame_count_ == 1) {
              return effect_display_duration_;
            }
            screen_ = SCREEN_PLI;
            if (SaberBase::Lockup()) {
              ShowFile(&IMG_lock, 3600000.0);
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

  void SetScreenNow(Screen screen) {
    millis_to_display_ = 0;
    screen_ = screen;
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

  void SB_On() override {
    ShowFile(&IMG_on, font_config.ProffieOSOnImageDuration);
  }

  void SB_Effect(EffectType effect, float location) override {
    switch (effect) {
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
    STDOUT.print("display fps: ");
    loop_counter_.Print();
    STDOUT.println("");
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

  void Loop() override {
    STATE_MACHINE_BEGIN();
    while (!i2cbus.inited()) YIELD();

    // Init sequence
    Send(DISPLAYOFF);                    // 0xAE
    Send(SETDISPLAYCLOCKDIV);            // 0xD5
    Send(0x80);                                  // the suggested ratio 0x80

    Send(SETMULTIPLEX);                  // 0xA8
    Send(HEIGHT - 1);

    Send(SETDISPLAYOFFSET);              // 0xD3
    Send(0x0);                                   // no offset
    Send(SETSTARTLINE | 0x0);            // line #0
    Send(CHARGEPUMP);                    // 0x8D
    Send(0x14);
    Send(MEMORYMODE);                    // 0x20
    Send(0x01);                          // vertical address mode
    #ifndef OLED_FLIP_180		 // normal OLED operation
    Send(SEGREMAP | 0x1);
    Send(COMSCANDEC);
    #else				 // allows for 180deg rotation of the OLED mapping
    Send(SEGREMAP);
    Send(COMSCANINC);
    #endif

    Send(SETCOMPINS);                    // 0xDA
    Send(0x02);  // may need to be 0x12 for some displays
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

    STDOUT.println("Display initialized.");
    screen_ = SCREEN_STARTUP;
    if (IMG_boot) {
      ShowFile(&IMG_boot, font_config.ProffieOSFontImageDuration);
    }

    while (true) {
      millis_to_display_ = FillFrameBuffer();
      frame_start_time_ = millis();
      lock_fb_ = true;
      frame_available_ = false;

      Send(COLUMNADDR);
      Send(0);   // Column start address (0 = reset)
      Send(WIDTH-1); // Column end address (127 = reset)

      Send(PAGEADDR);
      Send(0); // Page start address (0 = reset)
      switch (HEIGHT) {
        case 64:
          Send(7); // Page end address
          break;
        case 32:
          Send(3); // Page end address
          break;
        case 16:
          Send(1); // Page end address
          break;
        default:
          STDOUT.println("Unknown display height");
      }

      //STDOUT.println(TWSR & 0x3, DEC);

      // I2C
      for (i=0; i < WIDTH * HEIGHT / 8; ) {
        // send a bunch of data in one xmission
        Wire.beginTransmission(address_);
        Wire.write(0x40);
        for (uint8_t x=0; x<16; x++) {
          uint8_t b;
          switch (layout_) {
            default:
            case LAYOUT_NATIVE:
              b = ((unsigned char *)frame_buffer_)[i];
              break;
            case LAYOUT_PORTRAIT:
              if (!invert_y_) {
                b = ((unsigned char *)frame_buffer_)[511 - i];
              } else {
                b = ((unsigned char *)frame_buffer_)[i ^ 3];
              }
              break;
            case LAYOUT_LANDSCAPE: {
              int x = i >> 2;
              int y = ((i & 3) << 3) + 7;
              int delta_pos = -16;
              if (invert_y_) {
                y = 31 - y;
                delta_pos = 16;
              }
//            STDOUT << " LANDSCAPE DECODE!! x = " << x << " y = " << y << "\n";

              int shift = 7 - (x & 7);
              uint8_t *pos =
                ((unsigned char*)frame_buffer_) + ((x>>3) + (y<<4));
              b = 0;
              for (int j = 0; j < 8; j++) {
                b <<= 1;
                b |= (*pos >> shift) & 1;
                pos += delta_pos;
              }
            }
          }
          Wire.write(b ^ xor_);
          i++;
        }
        Wire.endTransmission();
        I2CUnlock(); do { YIELD(); } while (!I2CLock());
      }
      loop_counter_.Update();
      lock_fb_ = false;
      scheduleFillBuffer();
      I2CUnlock();
      while (millis() - frame_start_time_ < millis_to_display_) YIELD();
      do { YIELD(); } while (!I2CLock());
    }

    STATE_MACHINE_END();
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
          f->Seek(file_start + offset + width * height / 8 - 512);
      }
      if (width != 128 && width != 32) {
        STDOUT << "Wrong size image: " << width << "x" << height << "\n";
        return false;
      }
      if (width == 128) {
        layout_ = LAYOUT_LANDSCAPE;
        looped_frames_ = height / 32;
      } else {
        looped_frames_ = height / 128;
      }
      if (current_effect_ == &IMG_on) {
        looped_on_ = looped_frames_ > 1;
      }
    }
    // STDOUT << "ypos=" << ypos_ << " avail=" << f->Available() << "\n";
    if (f->Available() < sizeof(frame_buffer_)) return false;
    f->Read((uint8_t*)frame_buffer_, sizeof(frame_buffer_));
    ypos_++;
    if (looped_frames_ > 1) {
      if (ypos_ >= looped_frames_) {
        f->Seek(f->FileSize());
      } else {
        if (invert_y_) {
          // Seek two frames back, because BMP are backwards.
          f->Seek(f->Tell() - 1024);
        }
      }
    } else {
      if (file_end) f->Seek(file_end);
    }
    return true;
  }

  // AudioStreamWork implementation
  size_t space_available() const override {
    if (eof_ || lock_fb_) return 0;
    if (frame_available_) return 0;

    // Always low priority
    return 1;
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

  // TODO: Don't update the display when we don't need to
  // and return false here so that we can go into lower power modes.
  void SB_IsOn(bool* on) override {
    *on = true;
  }
private:
  uint16_t i;
  uint8_t xor_ = 0;
  bool invert_y_ = 0;
  volatile bool looped_on_ = false;
  volatile Effect* current_effect_;
  volatile float effect_display_duration_;
  uint32_t frame_buffer_[WIDTH];
  LoopCounter loop_counter_;
  char message_[32];
  uint32_t millis_to_display_;
  uint32_t frame_start_time_;
  Screen screen_;
  int32_t ypos_ = 0;
  volatile int32_t looped_frames_ = 0;
  volatile ScreenLayout layout_;
  uint32_t loop_start_;
  int32_t frame_count_ = 0;

  EffectFileReader file_;
  volatile bool frame_available_ = true;
  volatile bool eof_ = true;
  volatile bool lock_fb_ = false;
};

#endif
