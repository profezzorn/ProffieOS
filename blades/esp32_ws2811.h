#include "driver/rmt.h"

class RMTClient {
public:
  virtual bool rmt_run(rmt_channel_t channel) = 0;
  virtual bool rmt_done(rmt_channel_t channel) = 0;
  virtual size_t rmt_read(rmt_item32_t *dest, size_t wanted_num) { return 0; }
  virtual bool rmt_eof() { return true; }
  RMTClient* next_rmt_client = nullptr;
};

RMTClient* rmt_clients[8];

class RMTManager : public Looper {
public:
  const char* name() override { return "RMTManager"; }
  void Loop() override { run(); }
  void queue(RMTClient* client) {
#if 1  // debug    
    for (size_t i = 0; i < NELEM(rmt_clients); i++) {
      if (rmt_clients[i] == client) {
	STDERR << "QUEING RUNNING CLIENT!\n";
	while(true);
      }
    }
#endif
      
    client->next_rmt_client = nullptr;
    if (!pending_clients_) {
      last_pending_client_ = pending_clients_ = client;
    } else {
      PROFFIEOS_ASSERT(pending_clients_ != client);
      PROFFIEOS_ASSERT(last_pending_client_ != client);
      last_pending_client_->next_rmt_client = client;
      last_pending_client_ = client;
    }
    run();
  }
  void run() {
    // Free up anything that is finished.
    for (size_t i = 0; i < NELEM(rmt_clients); i++) {
      if (rmt_clients[i] && rmt_clients[i]->rmt_done((rmt_channel_t)i)) {
	rmt_clients[i] = nullptr;
      }
    }

    RMTClient* client = pending_clients_;
    if (!client) return;

    for (size_t i = 0; i < NELEM(rmt_clients); i++) {
      if (rmt_clients[i] == nullptr) {
	rmt_clients[i] = client;
	if (client->rmt_run((rmt_channel_t)i)) {
	  pending_clients_ = client->next_rmt_client;
	  client->next_rmt_client = nullptr;
	  if (!pending_clients_) {
	    last_pending_client_ = nullptr;
	    break;
	  }
	  client = pending_clients_;
	} else {
	  rmt_clients[i] = nullptr;
	}
      }
    }
  }
private:
  RMTClient* pending_clients_ = nullptr;
  RMTClient* last_pending_client_ = nullptr;
};

RMTManager rmt_manager;


static void IRAM_ATTR ws2811_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size, 
					 size_t wanted_num, size_t *translated_size, size_t *item_num) {
  RMTClient* client = (RMTClient*)src;
  *item_num = client->rmt_read(dest, wanted_num);
  *translated_size = client->rmt_eof() ? 1 : 0;
}

template<Color8::Byteorder BYTEORDER>
class RMTPinBase : public RMTClient, public WS2811PIN {
public:
  RMTPinBase(int LEDS, int PIN, int frequency, int reset_us, int t1h, int t0h) :
    num_leds_(0),
    pin_(PIN),
    frequency_(frequency),
    reset_us_(reset_us),
    t1h_(t1h),
    t0h_(t0h) {
    colors_ = (Color16*)malloc(sizeof(Color16) * LEDS);
    if (!colors_) return;

    num_leds_ = LEDS;
  }

  ~RMTPinBase() {
    free(colors_);
  }
  
  bool rmt_run(rmt_channel_t channel) override {
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX((gpio_num_t)pin_, channel);
    config.clk_div = 2;
    if (rmt_config(&config) != ESP_OK) {
      STDERR << "rmt_config failed\n";
      return false;
    }
    rmt_driver_install(channel, 0, 0);

    // Convert NS timings to ticks
    uint32_t counter_clk_hz = 0;
    if (rmt_get_counter_clock(channel, &counter_clk_hz) != ESP_OK) {
      STDERR << "rmt_counter_clock failed\n";
      rmt_driver_uninstall(channel);
      return false;
    }

    int ticks_per_loop = counter_clk_hz / frequency_;
    bit0.level0 = 1;
    bit0.duration0 = t0h_ * ticks_per_loop / 1250;  // 1250 nanos = 800Hz
    bit0.level1 = 0;
    bit0.duration1 = ticks_per_loop - t0h_ * ticks_per_loop / 1250;
    
    bit1.level0 = 1;
    bit1.duration0 = t1h_ * ticks_per_loop / 1250;
    bit1.level1 = 0;
    bit1.duration1 = ticks_per_loop - t1h_ * ticks_per_loop / 1250;

    uint32_t reset_ticks = reset_us_ / 1000000.0 * counter_clk_hz / 2;
    reset_code_.level0 = 0;
    reset_code_.duration0 = reset_ticks;
    reset_code_.level1 = 0;
    reset_code_.duration1 = reset_ticks;

    state_machine_.reset_state_machine();

    rmt_translator_init(channel, ws2811_rmt_adapter);
    if (rmt_write_sample(channel, (uint8_t*)this, (size_t)1, true) != ESP_OK) {
      STDERR << "rmt_write_sample failed\n";
      rmt_driver_uninstall(channel);
      return false;
    }
    return true;
  }

  bool rmt_done(rmt_channel_t channel) override {
    uint32_t status;
    rmt_get_status(channel, &status);
    if (status & 0x1000000) return false;

    rmt_driver_uninstall(channel);
    gpio_set_direction((gpio_num_t)pin_, GPIO_MODE_OUTPUT);
    done_ = true;
    return true;
  }

  bool IsReadyForBeginFrame() override { return true; }
  Color16* BeginFrame() override {
    frame_num_++;
    return colors_;
  }
  bool IsReadyForEndFrame() override { return done_; }
  void WaitUntilReadyForEndFrame() override {
    while (!IsReadyForEndFrame()) rmt_manager.run();
  }
  void EndFrame() override {
    WaitUntilReadyForEndFrame();
    done_ = false;
    rmt_manager.queue(this);
  }

  IRAM_ATTR bool rmt_eof() override {
    return state_machine_.done();
  }

  IRAM_ATTR size_t rmt_read(rmt_item32_t *dest, size_t wanted_num) override {
    ScopedCycleCounter cc(pixel_dma_interrupt_cycles);
    dest_ = dest;
    dest_end_ = dest + wanted_num;
    Fill();
    return dest_ - dest;
  }

#define RMT_ADAPTER_PUSH(X) do {		\
    if (dest_ == dest_end_) YIELD();		\
    *(dest_++) = (X);				\
  } while(0)

  IRAM_ATTR void Fill() {
    STATE_MACHINE_BEGIN();
    for (led = 0; led < num_leds_; led++) {
      color_ = colors_[led].dither(frame_num_, led);
      for (b = Color8::num_bytes(BYTEORDER) - 1; b >= 0; b--) {
	byte_ = color_.getByte(BYTEORDER, b);
	RMT_ADAPTER_PUSH((byte_ & 0x80) ? bit1 : bit0);
	RMT_ADAPTER_PUSH((byte_ & 0x40) ? bit1 : bit0);
	RMT_ADAPTER_PUSH((byte_ & 0x20) ? bit1 : bit0);
	RMT_ADAPTER_PUSH((byte_ & 0x10) ? bit1 : bit0);
	RMT_ADAPTER_PUSH((byte_ & 0x08) ? bit1 : bit0);
	RMT_ADAPTER_PUSH((byte_ & 0x04) ? bit1 : bit0);
	RMT_ADAPTER_PUSH((byte_ & 0x02) ? bit1 : bit0);
	RMT_ADAPTER_PUSH((byte_ & 0x01) ? bit1 : bit0);
      }
    }
    RMT_ADAPTER_PUSH(reset_code_);
    STATE_MACHINE_END();
  }

  int pin() const override { return pin_; }
  int num_leds() const override { return num_leds_; }
  Color8::Byteorder get_byteorder() const override { return BYTEORDER; }
  void Enable(bool on) override {
    pinMode(pin_, on ? OUTPUT : INPUT_ANALOG);
  }

  uint8_t byte_;
  Color8 color_;
  int led, b;

  rmt_item32_t bit0, bit1, reset_code_;
  rmt_item32_t *dest_, *dest_end_;
    
  bool done_ = true;
  int frame_num_ = 0;
  Color16* colors_;
  int num_leds_;
  int pin_;
  int frequency_;
  int reset_us_;
  int t1h_;
  int t0h_;
  StateMachineState state_machine_;
};

template<int LEDS, int PIN, Color8::Byteorder BYTEORDER, int frequency=800000, int reset_us=300, int t1h=294, int t0h=892>
class RMTWSPin : public RMTPinBase<BYTEORDER> {
public:  
  RMTWSPin() : RMTPinBase<BYTEORDER>(LEDS, PIN, frequency, reset_us, t1h, t0h) {}
};

