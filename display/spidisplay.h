#ifndef DISPLAY_SPIDISPLAY_H
#define DISPLAY_SPIDISPLAY_H

// Unfortunately, including SPI.h uses 3k even if if it's never actually used on Proffieboards.
#include <SPI.h>

constexpr uint8_t encode_sleep_time(int millis) {
  return
    0x80 + (
      millis < 32 ? millis :
      (millis / 10) < 32 ? (millis / 10 + 32) :
      (millis / 100) < 32 ? (millis / 100 + 64) :
      (millis / 1000 + 96));
}

constexpr int decode_sleep_time(uint8_t x) {
  return (x & 31) * ((x & 96) == 0 ? 1 :
		     (x & 96) == 32 ? 10 :
		     (x & 96) == 64 ? 100 :
		     1000);
}

template<uint8_t... BYTES> using SEND = ByteArray<sizeof...(BYTES), BYTES...>;
template<int millis> using SLEEP = ByteArray<encode_sleep_time(millis)>;

#define GENCOMMAND0(NAME, NUM) using NAME = SEND<NUM>
#define GENCOMMAND1(NAME, NUM) template<uint8_t ARG> using NAME = SEND<NUM, ARG>
#define GENCOMMAND2(NAME, NUM) template<uint8_t ARG1, uint8_t ARG2> using NAME = SEND<NUM, ARG1, ARG2>
#define GENCOMMAND3(NAME, NUM) template<uint8_t ARG1, uint8_t ARG2, uint8_t ARG3> using NAME = SEND<NUM, ARG1, ARG2, ARG3>
#define GENCOMMAND4(NAME, NUM) template<uint8_t ARG1, uint8_t ARG2, uint8_t ARG3, uint8_t ARG4> using NAME = SEND<NUM, ARG1, ARG2, ARG3, ARG4>
#define GENCOMMAND6(NAME, NUM) \
  template<uint8_t ARG1, uint8_t ARG2, uint8_t ARG3, uint8_t ARG4, uint8_t ARG5, uint8_t ARG6> \
  using NAME = SEND<NUM, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6>

#define GENCOMMAND16(NAME, NUM)						\
  template<uint8_t ARG1, uint8_t ARG2, uint8_t ARG3, uint8_t ARG4,	\
           uint8_t ARG5, uint8_t ARG6, uint8_t ARG7, uint8_t ARG8,	\
           uint8_t ARG9, uint8_t ARG10, uint8_t ARG11, uint8_t ARG12,	\
           uint8_t ARG13, uint8_t ARG14, uint8_t ARG15, uint8_t ARG16>	\
  using NAME = SEND<NUM, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9, ARG10, ARG11, ARG12, ARG13, ARG14, ARG15, ARG16>


struct ST77XXCommands {
  GENCOMMAND0(NOP,     0x00);
  GENCOMMAND0(SWRESET, 0x01);
  GENCOMMAND0(SLPIN,   0x10);
  GENCOMMAND0(SLPOUT,  0x11);
  GENCOMMAND0(NORON,   0x13);
  GENCOMMAND0(INVOFF,  0x20);
  GENCOMMAND0(INVON,   0x21);
  GENCOMMAND0(DISPOFF, 0x28);
  GENCOMMAND0(DISPON,  0x29);
  GENCOMMAND4(CASET,   0x2A);
  GENCOMMAND4(RASET,   0x2B);
  GENCOMMAND1(MADCTL,  0x36);
  GENCOMMAND0(MADCTL_0,  0x36);
  GENCOMMAND1(COLMOD,  0x3A);
  enum {
    ST77XX_MADCTL_MY = 0x80,
    ST77XX_MADCTL_MX = 0x40,
    ST77XX_MADCTL_MV = 0x20,
    ST77XX_MADCTL_ML = 0x10,
    ST77XX_MADCTL_RGB = 0x00,
    ST77XX_MADCTL_BGR = 0x08,

    ST77XX_RDID1 = 0xDA,
    ST77XX_RDID2 = 0xDB,
    ST77XX_RDID3 = 0xDC,
    ST77XX_RDID4 = 0xDD,

    ST77XX_CASET = 0x2A,
    ST77XX_RASET = 0x2B,
    ST77XX_RAMWR = 0x2C,
    ST77XX_DISPOFF = 0x28,
    ST77XX_DISPON = 0x29,
  };

  typedef MADCTL<                   ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB> Rotate0;
  typedef MADCTL<ST77XX_MADCTL_MV |                    ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB> Rotate1;
  typedef MADCTL<                                                         ST77XX_MADCTL_RGB> Rotate2;
  typedef MADCTL<ST77XX_MADCTL_MV | ST77XX_MADCTL_MX                    | ST77XX_MADCTL_RGB> Rotate3;
  typedef MADCTL<                                      ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB> Rotate4;
  typedef MADCTL<ST77XX_MADCTL_MV                                       | ST77XX_MADCTL_RGB> Rotate5;
  typedef MADCTL<                   ST77XX_MADCTL_MX                    | ST77XX_MADCTL_RGB> Rotate6;
  typedef MADCTL<ST77XX_MADCTL_MV | ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB> Rotate7;

  static const int MHZ = 1000000;
};

struct ST77XXBGR : public ST77XXCommands {
  typedef MADCTL<                   ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_BGR> Rotate0;
  typedef MADCTL<ST77XX_MADCTL_MV |                    ST77XX_MADCTL_MY | ST77XX_MADCTL_BGR> Rotate1;
  typedef MADCTL<                                                         ST77XX_MADCTL_BGR> Rotate2;
  typedef MADCTL<ST77XX_MADCTL_MV | ST77XX_MADCTL_MX                    | ST77XX_MADCTL_BGR> Rotate3;
  typedef MADCTL<                                      ST77XX_MADCTL_MY | ST77XX_MADCTL_BGR> Rotate4;
  typedef MADCTL<ST77XX_MADCTL_MV                                       | ST77XX_MADCTL_BGR> Rotate5;
  typedef MADCTL<                   ST77XX_MADCTL_MX                    | ST77XX_MADCTL_BGR> Rotate6;
  typedef MADCTL<ST77XX_MADCTL_MV | ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_BGR> Rotate7;
};

struct ST7735 : public ST77XXCommands {
  GENCOMMAND3(FRMCTR1, 0xB1);
  GENCOMMAND3(FRMCTR2, 0xB2);
  GENCOMMAND6(FRMCTR3, 0xB3);
  GENCOMMAND1(INVCTR,  0xB4);
  GENCOMMAND2(DISSET5, 0xB6);
  GENCOMMAND2(PWCTR1,  0xC0);
  GENCOMMAND3(PWCTR1_3,0xC0);
  GENCOMMAND1(PWCTR2,  0xC1);
  GENCOMMAND2(PWCTR3,  0xC2);
  GENCOMMAND2(PWCTR4,  0xC3);
  GENCOMMAND2(PWCTR5,  0xC4);
  GENCOMMAND2(VMCTR1,  0xC5);
  GENCOMMAND1(VMCTR1_1,  0xC5);
  GENCOMMAND2(PWCTR6,  0xFC);
  GENCOMMAND16(GMCTRN1,0xE0);
  GENCOMMAND16(GMCTRP1,0xE1);

  typedef ConcatByteArrays<
    SWRESET,
    SLEEP<50>,
    SLPOUT,
    SLEEP<500>,
    COLMOD<5>,
    SLEEP<10>,
    FRMCTR1<0, 6, 3>,
    SLEEP<10>,
    MADCTL<1>,
    DISSET5<0x15, 2>,
    INVCTR<0>,
    PWCTR1<2, 0x70>,
    SLEEP<10>,
    PWCTR2<5>,
    PWCTR3<1, 2>,
    VMCTR1<0x3c, 0x38>,
    SLEEP<10>,
    PWCTR6<0x11, 0x15>,
    GMCTRP1<0x09, 0x16, 0x09, 0x20, 0x21, 0x1B, 0x13, 0x19, 0x17, 0x15, 0x1E, 0x2B, 0x04, 0x05, 0x02, 0x0E>,
    GMCTRN1<0x0B, 0x14, 0x08, 0x1E, 0x22, 0x1D, 0x18, 0x1E, 0x1B, 0x1A, 0x24, 0x2B, 0x06, 0x06, 0x02, 0x0F>,
    SLEEP<10>,
    CASET<0x00, 0x02, 0x00, 0x81>,
    RASET<0x00, 0x02, 0x00, 0x81>,
    NORON,
    SLEEP<10>,
    DISPON,
    SLEEP<500>
    > BCMD;

  typedef ConcatByteArrays<
    SLEEP<5000>, // REMOVE ME LATER
    SWRESET,
    SLEEP<150>,
    SLPOUT,
    SLEEP<500>,
    FRMCTR1<0x01, 0x2C, 0x2D>,
    FRMCTR2<0x01, 0x2C, 0x2D>,
    FRMCTR3<0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D>,
    INVCTR<1>,
    PWCTR1_3<0xA2, 0x02, 0x84>,
    PWCTR2<0xC5>,
    PWCTR3<0x0A, 0>,
    PWCTR4<0x8A, 0x2A>,
    PWCTR5<0x8A, 0xEE>,
    VMCTR1_1<0x0E>,
    INVOFF,
    MADCTL_0,
    COLMOD<5>
    > RCMD1;

  typedef ConcatByteArrays<
    CASET<0, 2, 0, 0x7f + 2>,
    RASET<0, 1, 0, 0x9f+1>
    > RCMD2GREEN;

  typedef ConcatByteArrays<
    CASET<0, 0, 0, 0x7f>,
    RASET<0, 0, 0, 0x9f>
    > RCMD2RED;

  typedef ConcatByteArrays<
    CASET<0, 0, 0, 0x7f>,
    RASET<0, 0, 0, 0x7f>
    > RCMD2GREEN144;

  typedef ConcatByteArrays<
    CASET<0, 0, 0, 0x4f>,
    RASET<0, 0, 0, 0x9f>
    > RCMD2GREEN160x80;

  typedef ConcatByteArrays<
    INVON,
    CASET<0, 0, 0, 0x4f>,
    RASET<0, 0, 0, 0x9f>
    > RCMD2GREEN160x80plugin;

  typedef ConcatByteArrays<
    GMCTRP1<0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10>,
    GMCTRN1<0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10>,
    NORON,
    SLEEP<10>,
    DISPON,
    SLEEP<100>
    > RCMD3;

  typedef ConcatByteArrays<
    SLEEP<150>,
    SWRESET,
    SLEEP<150>,
    SLPOUT,
    SLEEP<150>,
    FRMCTR1<0x01, 0x2C, 0x2D>,
    FRMCTR2<0x01, 0x2C, 0x2D>,
    FRMCTR3<0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D>,
    INVCTR<1>,
    PWCTR1_3<0xA2, 0x02, 0x84>,
    PWCTR2<0xC5>,
    PWCTR3<0x0A, 0>,
    PWCTR4<0x8A, 0x2A>,
    PWCTR5<0x8A, 0xEE>,
    VMCTR1_1<0x0E>,
    INVOFF,
    MADCTL_0,
    COLMOD<5>,
    GMCTRP1<0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10>,
    GMCTRN1<0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10>,
    NORON,
    SLEEP<10>
    > RCMD;

  typedef ConcatByteArrays<
    DISPON,
    SLEEP<100>
    > ONCMD;

  typedef ConcatByteArrays<
    DISPOFF,
    SLEEP<100>
    > OFFCMD;

  static const int MAX_SPI_FREQUENCY = 16 * MHZ;
};

struct ST7789 : public ST77XXCommands {
  typedef ConcatByteArrays<
    SLEEP<30>,
    SWRESET,
    SLEEP<150>,
    SLPOUT,
    SLEEP<10>,
    COLMOD<0x55>,
    SLEEP<10>,
    MADCTL<8>,
    CASET<0, 0, 0, 240>,
    RASET<0, 0, (320>>8) , 320 & 0xff>,
    INVON,
    SLEEP<10>,
    NORON,
    SLEEP<10>
    > GENERIC_ST7789_STARTUP;

  typedef ConcatByteArrays<
    DISPON,
    SLEEP<10>
    > ONCMD;

  typedef ConcatByteArrays<
    DISPOFF,
    SLEEP<10>
    > OFFCMD;

  static const int MAX_SPI_FREQUENCY = 60 * MHZ;
};

#if VERSION_MAJOR == 6  // v3 proffieboard
template<int MAX_SPI_FREQUENCY=1000000000>
struct StandardDisplayAdapter {
  template<class CHIP>
  struct Impl : private CHIP {
    static SPIClass& spi() { return SPI; }  // Data2=MISO, Data3=MOSI, Free1=SCK  (Figre out how to not use Data2)
    static void begin() {
#ifdef SPI_HAS_BEGIN_OUTPUT_ONLY      
      SPI.begin_output_only();
#else      
      SPI.begin();
#endif      
      SPISettings settings(std::min(MAX_SPI_FREQUENCY, CHIP::MAX_SPI_FREQUENCY), MSBFIRST, SPI_MODE0);
      spi().beginTransaction(settings);
    }
    static void end() { SPI.end(); }
    static const int RESET = -1;            //
    static const int LIGHT = blade6Pin;     // Free2 (backlight, PWM)
    static const int DC = blade7Pin;        // Free3
    static void beginTransaction() {
      // Do nothing, this is a dedicated SPI bus.
    }
    static void endTransaction() {
      // Do nothing, this is a dedicated SPI bus.
    }
    USE_PIN_OUTPUT(DC, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(LIGHT, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(RESET, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(blade2Pin, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(blade3Pin, PO_SubSystems::PO_SPI);
  };
};

template<int MAX_SPI_FREQUENCY=1000000000>
struct StandardDisplayAdapterM3 {
  template<class CHIP>
  struct Impl : private CHIP {
    static SPIClass& spi() { return SPI; }  // Data2=MISO, Data3=MOSI, Free1=SCK  (Figre out how to not use Data2)
    static void begin() {
      // SPI bus is dedicated to display, so we'll need to call SPI.begin()
#ifdef SPI_HAS_BEGIN_OUTPUT_ONLY      
      SPI.begin_output_only();
#else      
      SPI.begin();
#endif      
      SPISettings settings(std::min(MAX_SPI_FREQUENCY, CHIP::MAX_SPI_FREQUENCY), MSBFIRST, SPI_MODE3);
      spi().beginTransaction(settings);
    }
    static void end() { SPI.end(); }
    static const int RESET = -1;            //
    static const int LIGHT = blade6Pin;     // Free2 (backlight, PWM)
    static const int DC = blade7Pin;        // Free3
    static void beginTransaction() {
      // Do nothing, this is a dedicated SPI bus.
    }
    static void endTransaction() {
      // Do nothing, this is a dedicated SPI bus.
    }
    USE_PIN_OUTPUT(DC, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(LIGHT, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(RESET, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(blade2Pin, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(blade3Pin, PO_SubSystems::PO_SPI);
  };
};

template<int CS, int MAX_SPI_FREQUENCY=1000000000, bool SPI_SHARED = false>
struct CSDisplayAdapter {
  template<class CHIP>
  struct Impl : private CHIP {
    static SPIClass& spi() { return SPI; }  // Data2=MISO, Data3=MOSI, Free1=SCK  (Figre out how to not use Data2)
    static void begin() {
      // If the SPI bus is shared with other devices, then the user must call SPI.begin()
      if (!SPI_SHARED) {
#ifdef SPI_HAS_BEGIN_OUTPUT_ONLY      
	SPI.begin_output_only();
#else      
	SPI.begin();
#endif      
      }
      digitalWrite(CS, HIGH);
      pinMode(CS, OUTPUT);
    }
    static void end() { SPI.end(); }
    static const int RESET = -1;            //
    static const int LIGHT = blade6Pin;     // Free2 (backlight, PWM)
    static const int DC = blade7Pin;        // Free3
    static void beginTransaction() {
//    stm32l4_system_sysclk_configure(_SYSTEM_CORE_CLOCK_, _SYSTEM_CORE_CLOCK_/2, _SYSTEM_CORE_CLOCK_);
      SPISettings settings(std::min(MAX_SPI_FREQUENCY, CHIP::MAX_SPI_FREQUENCY), MSBFIRST, SPI_MODE0);
      spi().beginTransaction(settings);
      digitalWrite(CS, LOW);
    }
    static void endTransaction() {
      digitalWrite(CS, HIGH);
      spi().endTransaction();
    }
    USE_PIN_OUTPUT(CS, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(DC, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(LIGHT, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(RESET, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(blade2Pin, PO_SubSystems::PO_SPI);
    USE_PIN_OUTPUT(blade3Pin, PO_SubSystems::PO_SPI);
  };
};
#else
class StandardDisplayAdapter {
  // No solution for other boards yet.
};
#endif

struct TransferDoneInterface {
  virtual void transferDone() = 0;
};

struct SpiIrqHelper {
  static void cb0() { o[0]->transferDone(); }
  static void cb1() { o[1]->transferDone(); }
  static void cb2() { o[2]->transferDone(); }
  static void cb3() { o[3]->transferDone(); }
  static void cb4() { o[4]->transferDone(); }

  typedef void (*cbp)(void);
  
  cbp getCallback(int i) {
    switch (i) {
      case 0: return cb0;
      case 1: return cb1;
      case 2: return cb2;
      case 3: return cb3;
      case 4: return cb4;
    }
  }
  cbp getCallback(TransferDoneInterface* obj) {
    for (int i = 0; i < 5; i++) {
      if (o[i] == nullptr) {
	o[i] = obj;
	return getCallback(i);
      }
    }
    STDERR << "Too many SPI displays!";
    PROFFIEOS_ASSERT(false);
    return cb0;
  }
  void freeCallback(TransferDoneInterface* obj) {
    for (int i = 0; i < 5; i++) {
      if (o[i] == obj) {
	o[i] = nullptr;
	return;
      }
    }
  }
  static TransferDoneInterface* volatile o[5];
};

TransferDoneInterface* volatile SpiIrqHelper::o[5] = {0};

SpiIrqHelper spi_irq_helper;

// TODO: 40Mhz
// TODO: show input/output buffer state in "top"

template<int X, int Y>
struct SizeT {
  static const int x = X;
  static const int y = Y;
  typedef SizeT<Y, X> rotate;
};


template<int TOP, int BOTTOM, int LEFT, int RIGHT>
struct InsetT {
  static const int top = TOP;
  static const int bottom = BOTTOM;
  static const int left = LEFT;
  static const int right = RIGHT;

  template<class SIZE>
  using size = SizeT<SIZE::x - left - right, SIZE::y - top - bottom>;
};


template<int ROTATION = 0, class Insets = InsetT<0,0,0,0>>
struct DisplayConfig {
  static const int rotation = ROTATION;
  typedef Insets insets;


  template<int rot, class SIZE> using Rotate = SizeT<
    (rot & 1) ? SIZE::y : SIZE::x,
    (rot & 1) ? SIZE::x : SIZE::y>;

  template<class SIZE>
  using size = typename Insets::template size<Rotate<rotation, SIZE>>;

  template<int rot, class CHIP> struct RotationCmdSelector {};
  template<class CHIP> struct RotationCmdSelector<0, CHIP> { using cmd = typename CHIP::Rotate0; };
  template<class CHIP> struct RotationCmdSelector<1, CHIP> { using cmd = typename CHIP::Rotate1; };
  template<class CHIP> struct RotationCmdSelector<2, CHIP> { using cmd = typename CHIP::Rotate2; };
  template<class CHIP> struct RotationCmdSelector<3, CHIP> { using cmd = typename CHIP::Rotate3; };
  template<class CHIP> struct RotationCmdSelector<4, CHIP> { using cmd = typename CHIP::Rotate4; };
  template<class CHIP> struct RotationCmdSelector<5, CHIP> { using cmd = typename CHIP::Rotate5; };
  template<class CHIP> struct RotationCmdSelector<6, CHIP> { using cmd = typename CHIP::Rotate6; };
  template<class CHIP> struct RotationCmdSelector<7, CHIP> { using cmd = typename CHIP::Rotate7; };

  template<class CHIP> using rotation_cmd = typename RotationCmdSelector<rotation, CHIP>::cmd;
};

template<int LAYERS, class CONFIG, class SIZE, class Adapter, class CHIP>
struct DisplayConfigHelper {
  typedef typename CONFIG::template size<SIZE> size;
  typedef RGB565Frame<size::x, size::y, LAYERS> frame;
  typedef CHIP chip;
  typedef typename Adapter::template Impl<CHIP> SA;
};


// template<
//   int LAYERS,
//   CONFIG = DisplayConfig<int rotation, Crop = Insets<0,0,0,0>>
//   SA = StandardDisplayAdapter < spi_speed >,
//  >
       
// Note: Insets are applied *after* rotation.
template<int LAYERS,
	 class CONFIG,
	 class HELPER,
	 class InitSequence,
	 class OnSequence,
	 class OffSequence>
class SPIDisplayBase : public HELPER::frame, public Looper, public TransferDoneInterface, public CommandParser {
public:
  typedef typename HELPER::size SIZE;
  typedef typename HELPER::SA SA;
  static const int WIDTH = SIZE::x;
  static const int HEIGHT = SIZE::y;

  const char* name() override { return "SPIDisplay_AdaFruit358"; }
  
  SPIDisplayBase() {
    cb = spi_irq_helper.getCallback(this);
  }

  ~SPIDisplayBase() {
    spi_irq_helper.freeCallback(this);
  }

  
  void initDisplay() override {
    TRACE(RGB565, "initDisplay");
    SA::begin();
    PVLOG_STATUS << "Initializing display " << WIDTH << " x " << HEIGHT << "\n";
    backlight_.Activate();
    backlight_.set(0);
    digitalWrite(SA::DC, HIGH);
    pinMode(SA::DC, OUTPUT);

    to_send_ = InitSequence::data;
    to_send_end_ = InitSequence::data + sizeof(InitSequence::data);
  }

  void enableDisplay() override {
    TRACE(RGB565, "enableDisplay");
    to_send_ = OnSequence::data;
    to_send_end_ = OnSequence::data + sizeof(OnSequence::data);
  }

  void enableBacklight() override {
    TRACE(RGB565, "enableBacklight");
    backlight_.set(65536);
  }

  void disableDisplay() override {
    TRACE(RGB565, "disableDisplay");
    backlight_.set(0);
    to_send_ = OffSequence::data;
    to_send_end_ = OffSequence::data + sizeof(OffSequence::data);
  }

  void sendCommand(uint8_t cmd, const uint8_t* argv, int argc) {
    TRACE(RGB565, "sendCommand");
    SA::beginTransaction();
    digitalWrite(SA::DC, LOW); // Command mode
    SA::spi().write(cmd);
    digitalWrite(SA::DC, HIGH); // Command mode off
    for (int i =0; i < argc; i++) {
      SA::spi().write(*(argv++));
    }
    SA::endTransaction();
  }

  virtual void startFrame() = 0;

  bool do_end_transaction = false;

  void transferDone() override {
    SA::endTransaction();
//    TRACE(RGB565, "transferDone");
    HELPER::frame::output_buffers_.pop(1);
    transferring_.set(false);
    startTransfer();
  }

  void startTransfer() override {
    extern void ClockControl_AvoidSleep();
    ClockControl_AvoidSleep();
    // This is atomic, so only one "thread" can execute at a time below this.
    if (!transferring_.cas(false, true)) return;
    if (HELPER::frame::output_buffers_.size() == 0 || !HELPER::frame::output_buffers_.data()->done.get()) {
      transferring_= false;
      return;
    }
    if (HELPER::frame::output_buffers_.data()->rownum == 0) {
      PVLOG_VERBOSE << "Start new frame, " << (HELPER::frame::output_buffers_.data()->chunk.end() - HELPER::frame::output_buffers_.data()->chunk.begin()) << " bytes pixel0 = " << *HELPER::frame::output_buffers_.data()->chunk.begin() << "\n";
      startFrame();
    }
    SA::beginTransaction();
    
    uint16_t *data = HELPER::frame::output_buffers_.data()->chunk.begin();
    bool ret = SA::spi().transfer(data,
			    nullptr,
			    (HELPER::frame::output_buffers_.data()->chunk.end() - HELPER::frame::output_buffers_.data()->chunk.begin()) * 2,
				  cb);
    if (!ret) {
      transferring_ = false;
      SA::endTransaction();
      STDERR << "spi transfer failed.";
//	TRACE(RGB565, "startTransfer7");
    }
  }

  void fixByteOrder() override {
    // Need to flip the byte order
    // TODO: Is it possible to transfer bytes backwards to do this?
    uint16_t *data = HELPER::frame::current_output_buffer_->chunk.begin();
    uint32_t *data32 = reinterpret_cast<uint32_t*>(data);
    for (int i = 0; i < WIDTH/2; i++) data32[i] = __REV16(data32[i]);
  }

  void swapBuffers() override {
    TRACE(RGB565, "swapBuffers");
  }
  
  void Loop() override {
    while (to_send_ < to_send_end_) {
      if (wait_time_) {
	if (millis() - wait_start_ < wait_time_) {
	  return;
	}
	wait_time_ = 0;
      }

      uint8_t cmd = *(to_send_++);
      if (cmd & 0x80) {
	wait_start_ = millis();
	wait_time_ = decode_sleep_time(cmd);
	PVLOG_DEBUG << "SPI sleeping " << wait_time_ << "ms code = " << cmd << " .\n";
	return;
      }
      PVLOG_DEBUG << "Sending SPI command with " << (cmd-1) << " arguments. command = " << to_send_[0] << "\n";
      sendCommand(to_send_[0], to_send_+1, cmd - 1);
      to_send_ += cmd;
    }
    this->frame_loop();
    // HELPER::frame::frame_loop();
  }

  bool Parse(const char* cmd, const char* arg) override {
#ifdef ENABLE_DEVELOPER_COMMANDS
    if (!strcmp(cmd, "testframe")) {
      startFrame();
      for (size_t y = 0; y < WIDTH; y++) {
	for (size_t x = 0; x < WIDTH; x++)  {
	  SA::spi().write(x + y);
	  SA::spi().write(y - x);
	}
      }
      return true;
    }
    if (!strcmp(cmd, "displaystate")) {
      STDOUT << "to_send: " << (to_send_end_ - to_send_) << "\n";
      STDOUT << "PCLK1: " << stm32l4_system_pclk1() << " PCLK2: " << stm32l4_system_pclk2() << "\n";;
      HELPER::frame::dumpstate();
      return true;
    }
#endif
    return false;
  }

  bool ready_ = false;
  const uint8_t* to_send_ = nullptr;
  const uint8_t* to_send_end_ = nullptr;
  uint32_t wait_start_;
  uint32_t wait_time_ = 0;
  POAtomic<bool> transferring_;

  // TODO: Allow FET and non-pwm pins, probably by improving SimplePWMPin<>.
  SimplePWMPin<SA::LIGHT> backlight_;

  SpiIrqHelper::cbp cb;
};

template<int LAYERS,
	 class CONFIG,
	 class HELPER,
	 class InitSequence,
	 class OnSequence,
	 class OffSequence>
class SPIDisplay77XX : public SPIDisplayBase<LAYERS, CONFIG, HELPER, InitSequence, OnSequence, OffSequence> {
public:
  void startFrame() override {
    TRACE(RGB565, "startFrame");
    uint8_t tmp[4];
    uint16_t a;

    a = CONFIG::insets::left;
    tmp[0] = a >> 8;
    tmp[1] = a & 0xff;
    a += this->WIDTH - 1;
    tmp[2] = a >> 8;
    tmp[3] = a & 0xff;
    
    this->sendCommand(ST77XXCommands::ST77XX_CASET, tmp, 4);

    a = CONFIG::insets::top;
    tmp[0] = a >> 8;
    tmp[1] = a & 0xff;
    a += this->HEIGHT - 1;
    tmp[2] = a >> 8;
    tmp[3] = a & 0xff;
    
    this->sendCommand(ST77XXCommands::ST77XX_RASET, tmp, 4);
    this->sendCommand(ST77XXCommands::ST77XX_RAMWR, tmp, 0);
  }
};

template<int LAYERS,
	 class CONFIG = DisplayConfig<1>,
	 class SA = StandardDisplayAdapter<>>
using SPIDisplay_AdaFruit358 = SPIDisplay77XX<
  LAYERS,
  CONFIG,
  // HELPER
  DisplayConfigHelper<LAYERS, CONFIG, SizeT<128, 160>, SA, ST7735>,
  // InitSequence
  ConcatByteArrays<ST7735::RCMD, typename CONFIG::template rotation_cmd<ST7735> >,
  // OnSequence
  ST7735::ONCMD,
  // Offsequence
  ST7735::OFFCMD
  >;


template<int LAYERS,
	 class CONFIG = DisplayConfig<1, InsetT<24,24,0,0>>,
	 class SA = StandardDisplayAdapter<>>
using SPIDisplay_DFRobot096 = SPIDisplay77XX<
  LAYERS,
  CONFIG,
  // HELPER
  DisplayConfigHelper<LAYERS, CONFIG, SizeT<128, 160>, SA, ST7735>,
  // InitSequence
  ConcatByteArrays<ST7735::RCMD, typename CONFIG::template rotation_cmd<ST77XXBGR> >,
  // OnSequence
  ST7735::ONCMD,
  // Offsequence
  ST7735::OFFCMD
  >;


template<int LAYERS,
	 class CONFIG = DisplayConfig<0, InsetT<0,0,20,20>>,
	 class SA = CSDisplayAdapter<blade4Pin>>
using SPIDisplay_AdaFruit5206 = SPIDisplay77XX<
  LAYERS,
  CONFIG,
  // HELPER
  DisplayConfigHelper<LAYERS, CONFIG, SizeT<320, 240>, SA, ST7789>,
  // InitSequence
  ConcatByteArrays<ST7789::GENERIC_ST7789_STARTUP, typename CONFIG::template rotation_cmd<ST7789> > ,
  // OnSequence
  ST7789::ONCMD,
  // Offsequence
  ST7789::OFFCMD
  >;


template<int LAYERS,
	 class CONFIG = DisplayConfig<1, InsetT<0,0,0,0>>,
	 class SA = CSDisplayAdapter<blade4Pin>>
using SPIDisplay_AdaFruit4311 = SPIDisplay77XX<
  LAYERS,
  CONFIG,
  // HELPER
  DisplayConfigHelper<LAYERS, CONFIG, SizeT<240, 320>, SA, ST7789>,
  // InitSequence
  ConcatByteArrays<ST7789::GENERIC_ST7789_STARTUP, typename CONFIG::template rotation_cmd<ST7789> > ,
  // OnSequence
  ST7789::ONCMD,
  // Offsequence
  ST7789::OFFCMD
  >;


#endif
