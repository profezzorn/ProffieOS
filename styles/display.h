#ifndef STYLE_DISPLAY_H
#define STYLE_DISPLAY_H

#include <type_traits>
#include "../display/rgb565frame.h"

// Usage: DisplayStyle<MYDISPLAY>
// MYDISPLAY: named instance
// Returns: COLOR
// This style requires a little more work than regular styles.
// First you would need to declare an in-memory display.
// To do this, we would put this in the CONFIG_STYLES section:
//
//     InMemoryDisplay<8, 8, 3> my_small_display;
//
// Then you need a controller for it:
//
//     StandarColorDisplayController<8, 8> my_small_display_controller(&my_small_display);
//
// Now we need give this display a name we can use in a style:
//
//     NAME_INSTANCE(my_small_display, MYDISPLAY);
//
// Now we we can use this inside a style template:
//
//     DisplayStyle<MYDISPLAY>
//
// Briefly, the display controller reads SCR files and tells
// the in-memory display which PQF files to read. The PQF
// files are rendered into an in-memory frame buffer.
// DisplayStyle is used to read from that frame buffer.
// Note that it's totally ok to use this with regular
// blades, just make the height or the width of the
// display equal to 1.

template<int WIDTH, int HEIGHT, int LAYERS, int NBUF=3>
class InMemoryDisplay : public RGB565Frame<WIDTH, HEIGHT, LAYERS>, public Looper {
  typedef RGB565Frame<WIDTH, HEIGHT, LAYERS> SUPER;
public:
  struct FB {
    uint16_t pixels[HEIGHT][WIDTH];
    uint16_t refs = 0;
    void AddRef() { refs++; }
    void SubRef() { refs--; }
    void clear() {
      memset(pixels, 0, sizeof(pixels));
    }
    Color16 getColor(int x, int y) {
      uint16_t pixel = pixels[y][x];
      int red = pixel >> 11;
      int green = (pixel >> 5) & 0x3f;
      int blue = pixel & 0x1f;
      red = (red * 0x8421) >> 4;
      green = (green * 0x1041) >> 2;
      blue = (blue * 0x8421) >> 4;
      return Color16(red, green, blue);
    }
  };
  typedef RefPtr<FB> FrameRef;
  const char* name() override { return "InMemoryDisplay"; }
  InMemoryDisplay() : Looper(HFLINK) {
  }
  void initDisplay() override {
    for (int i = 0; i < NBUF; i++) fb_[i].clear();
    target_buffer_ = FrameRef(fb_);
  }
  void enableDisplay() override {}
  void disableDisplay() override {}
  void enableBacklight() override {}
  void startTransfer() override {
    while (SUPER::output_buffers_.size() && SUPER::output_buffers_.data()->done.get()) {
      uint16_t *data = SUPER::output_buffers_.data()->chunk.begin();
      size_t num_pixels = SUPER::output_buffers_.data()->chunk.end() - SUPER::output_buffers_.data()->chunk.begin();
      memcpy(target_buffer_->pixels[SUPER::output_buffers_.data()->rownum], data, num_pixels * 2);
      SUPER::output_buffers_.pop(1);
    }
  }
  void fixByteOrder() override {}

  void swapBuffers() override {
    active_buffer_ = target_buffer_;
    target_buffer_ = FrameRef(fb_ + next_buffer());
  }
  
  void Loop() override { SUPER::frame_loop(); }
  FrameRef StartFrame() {
    return active_buffer_;
  }
private:
  int next_buffer() {
    for (size_t b = 0; b < NBUF; b++) if (fb_[b].refs == 0) return b;
    int t = 0;
    if (target_buffer_) t = target_buffer_.get() - fb_;
    return (t + 1) % NBUF;
  }
  FrameRef target_buffer_;
  FrameRef active_buffer_;
  FB fb_[NBUF];
};


// TODO: Move to common/malloc_helper.h

#define NAME_INSTANCE(I, T)						\
struct T;								\
auto getInstance(T*) -> decltype(& I) { return & I; }

template<class I, bool ZZ=true, int rot=0>
class DisplayStyle {
public:
typedef typename std::remove_reference<decltype( *getInstance((I*)0) )>::type DisplayType;
  typedef typename DisplayType::FrameRef FrameRef;
  const uint32_t W = DisplayType::W;
  const uint32_t H = DisplayType::H;

  void run(BladeBase* blade) {
    frame_ = getInstance((I*)0)->StartFrame();
  }
  SimpleColor getColor(int x, int y) {
    if (x > (int)W || y > (int)H || !frame_) return Color16(0,0,0);
    return frame_->getColor(x, y);
  }
  SimpleColor getColor(int led) {
    int x = led % W;
    int y = led / W;
    if (ZZ && (y & 1)) x = W - x;
    if (rot & 1) x = W - x;
    if (rot & 2) y = H - y;
    return (rot & 4) ? getColor(y, x) : getColor(x, y);
  }
private:
  FrameRef frame_;
};

#endif
