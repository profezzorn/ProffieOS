#ifndef PROFFIEOS_BLADES_TEENSY4_WS2811_H
#define PROFFIEOS_BLADES_TEENSY4_WS2811_H

#include <OctoWS2811.h>

#ifdef DEBUG
#define PROFFIEOS_ASSERT(X) do {					\
  if (!(X)) {								\
    interrupts();							\
    if (!(X)) STDERR << "ASSERT " << #X << " FAILED @ " << __FILE__ << ":" << __LINE__ << "\n"; \
    while(true);							\
  }									\
} while(0)
#else
#define PROFFIEOS_ASSERT(X) do {} while(0)
#endif

DMAMEM int displayMemoryT4[maxLedsPerStrip * 8 + 1];
OctoWS2811 octows2811(1, displayMemoryT4, 0);
			 
class OctoWS2811Client {
public:
  virtual int pin() const = 0;
  virtual int config() const = 0;
  virtual int bytes() const = 0;
  virtual void fill(uint8_t *data) = 0;
  virtual void done() = 0;
  OctoWS2811Client* next_ws2811_client_ = nullptr;
};

class OctopodWSEngine : public Looper {
public:
  const char* name() override { return "OctopodWSEngine"; }
  void queue(OctoWS2811Client* client) {
    client->next_ws2811_client_ = nullptr;
    if (!client_) {
      last_client_ = client_ = client;
    } else {
      PROFFIEOS_ASSERT(client_ != client);
      PROFFIEOS_ASSERT(last_client_ != client);
      last_client_->next_ws2811_client_ = client;
      last_client_ = client;
    }
    run();
  }

  void Setup() override {
    octows2811.begin();
  }
  void Loop() override { run(); }

  void run() {
    if (octows2811.busy()) return;
    if (!client_) return;
    int num_strings = 0;
    int config = client_->config();
    OctoWS2811Client* next_batch = nullptr;
    int max_bytes = 0;
    
    for (OctoWS2811Client *client = client_; client; client = client->next_ws2811_client_) {
      next_batch = client_;
      
      if (client->config() != config) break;
      if (num_strings >= (int)NELEM(pinList)) break;

      // We round up to 12 bytes to accommodate both RGB and RGBW pixels.
      int bytes = std::max<int>(max_bytes, (client->bytes() + 11) / 12 * 12);
      if (bytes * (num_strings + 1) > (int)sizeof(displayMemoryT4)) break;
      
      next_batch = nullptr;

      max_bytes = bytes;
      pinList[num_strings] = client->pin();
      pinMode(client->pin(), OUTPUT);
      num_strings++;
    }

    memset(displayMemoryT4, 0, num_strings * max_bytes);
    // Abusing the library by calling begin() again.
    octows2811.begin(max_bytes / 3, displayMemoryT4, 0, config, num_strings, pinList);


    int i = 0;;
    for (OctoWS2811Client *client = client_; client != next_batch; client = client->next_ws2811_client_) {
//      client->fill(((uint8_t *)displayMemoryT4) + (i+1) * max_bytes - client->bytes());
      client->fill(((uint8_t *)displayMemoryT4) + i * max_bytes);
      client->done();
      i++;
    }
    client_ = next_batch;
    if (!client_) last_client_ = nullptr;
    octows2811.show();
  }

private:
  OctoWS2811Client* client_ = nullptr;
  OctoWS2811Client* last_client_ = nullptr;
  uint8_t pinList[16];
};

OctopodWSEngine octopodWSEngine;

template<Color8::Byteorder BYTEORDER>
class OctopodWSPinBase : public OctoWS2811Client, public WS2811PIN {
public:
  OctopodWSPinBase(int LEDS, int PIN, int frequency) :
    num_leds_(LEDS), pin_(PIN), frequency_(frequency) {
    colors_ = (Color16*)malloc(sizeof(Color16) * LEDS);
    if (!colors_) {
      num_leds_ = 0;
    }
  }
  ~OctopodWSPinBase() {
    free(colors_);
  }
  bool IsReadyForBeginFrame() override { return true; }
  Color16* BeginFrame() override {
    frame_num_++;
    return colors_;
  }
  bool IsReadyForEndFrame() override { return done_; }
  void WaitUntilReadyForEndFrame() override {
    while (!IsReadyForEndFrame()) octopodWSEngine.run();
  }
  void EndFrame() override {
    WaitUntilReadyForEndFrame();
    done_ = false;
    octopodWSEngine.queue(this);
  }

  int bytes() const override { return num_leds_ * Color8::num_bytes(BYTEORDER); }
  void fill(uint8_t *output) override {
    for (int j = 0; j < num_leds_; j++) {
      Color8 color = colors_[j].dither(frame_num_, j);
      for (int i = Color8::num_bytes(BYTEORDER) - 1; i >= 0; i--) {
	*(output++) = color.getByte(BYTEORDER, i);
      }
    }
  }

  int config() const override {
    if (frequency_ < 600000) {
      return WS2811_RGB | WS2811_400kHz;
    }
    return WS2811_RGB | WS2813_800kHz;
  }
  void done() override { done_ = true; }
  int num_leds() const override { return num_leds_; }
  int pin() const override { return pin_; }
  Color8::Byteorder get_byteorder() const override { return BYTEORDER; }
  void Enable(bool on) override {
    pinMode(pin_, on ? OUTPUT : INPUT_ANALOG);
  }

  bool done_ = true;
  Color16* colors_;
  int num_leds_;
  uint8_t pin_;
  uint32_t frequency_;
  uint32_t frame_num_;
};

template<int LEDS, int PIN, Color8::Byteorder BYTEORDER, int frequency=800000, int reset_us=300, int t1h=294, int t0h=892>
class OctopodWSPin : public OctopodWSPinBase<BYTEORDER> {
public:  
  OctopodWSPin() : OctopodWSPinBase<BYTEORDER>(LEDS, PIN, frequency) {}
};

#endif
