#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <memory.h>

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))

#include "monoframe.h"

template<int WIDTH, class col_t>
class LandScapeFrame : public MonoFrame<WIDTH, col_t> {
public:
  static const int BYTES_PER_ROW = WIDTH / 8;
  static const int W = WIDTH;
  static const int H = MonoFrame<WIDTH,col_t>::HEIGHT;
  bool GetPixel(int x, int y) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < W);
    assert(y < H);
    uint8_t& byte = ((uint8_t*)MonoFrame<WIDTH,col_t>::frame_buffer_)[y * BYTES_PER_ROW + x / 8];
    uint8_t bit = (1 << (7 - x % 8));
    return !!(byte & bit);
  }

  void SetPixel(int x, int y, bool p) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < W);
    assert(y < H);
    uint8_t& byte = ((uint8_t*)MonoFrame<WIDTH,col_t>::frame_buffer_)[y * BYTES_PER_ROW + x / 8];
    uint8_t bit = (1 << (7 - x % 8));
    if (p) byte |= bit;
  }

  MonoFrame<WIDTH, col_t> Convert() {
    MonoFrame<WIDTH, col_t> ret;
    memcpy(ret.frame_buffer_, MonoFrame<WIDTH,col_t>::frame_buffer_, sizeof(MonoFrame<WIDTH,col_t>::frame_buffer_));
    ret.ConvertLandscape(false);
    return ret;
  }

  bool Compare(MonoFrame<WIDTH, col_t>& other) {
    for (int y = 0; y < MonoFrame<WIDTH,col_t>::HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {
	if (other.GetPixel(x, y) != GetPixel(x, y)) {
	  return false;
	}
      }
    }
    return true;
  }
};

template<int WIDTH, class col_t>
class PortraitFrame : public MonoFrame<WIDTH, col_t> {
public:
  static const int BYTES_PER_ROW = sizeof(col_t);
  static const int W = MonoFrame<WIDTH,col_t>::HEIGHT;
  static const int H = WIDTH;
  bool GetPixel(int x, int y) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < W);
    assert(y < H);
    uint8_t& byte = ((uint8_t*)MonoFrame<WIDTH,col_t>::frame_buffer_)[y * BYTES_PER_ROW + x / 8];
    uint8_t bit = (1 << (7 - x % 8));
    return !!(byte & bit);
  }

  void SetPixel(int x, int y, bool p) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < W);
    assert(y < H);
    uint8_t& byte = ((uint8_t*)MonoFrame<WIDTH,col_t>::frame_buffer_)[y * BYTES_PER_ROW + x / 8];
    uint8_t bit = (1 << (7 - x % 8));
    if (p) byte |= bit;
  }

  MonoFrame<WIDTH, col_t> Convert() {
    MonoFrame<WIDTH, col_t> ret;
    memcpy(ret.frame_buffer_, MonoFrame<WIDTH,col_t>::frame_buffer_, sizeof(MonoFrame<WIDTH,col_t>::frame_buffer_));
    ret.ConvertPortrait();
    return ret;
  }

  bool Compare(MonoFrame<WIDTH, col_t>& other) {
    for (int y = 0; y < MonoFrame<WIDTH,col_t>::HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {
	if (other.GetPixel(x, y) != GetPixel(MonoFrame<WIDTH,col_t>::HEIGHT - 1 - y, x)) {
	  return false;
	}
      }
    }
    return true;
  }
};

template<class T>
void ConvertAndCompare(T& frame) {
  auto tmp = frame.Convert();

  if (!frame.Compare(tmp)) {
    fprintf(stderr, "Compare failed\n");
    fprintf(stderr, "input...\n");

    for (int y = 0; y < T::H; y++) {
      for (int x = 0; x < T::W; x++) {
	fprintf(stderr, "%s", frame.GetPixel(x, y) ? "##" : "  ");
      }
      fprintf(stderr, "\n");
    }

    fprintf(stderr, "output...\n");

    for (int y = 0; y < T::HEIGHT; y++) {
      for (int x = 0; x < T::WIDTH; x++) {
	fprintf(stderr, "%s", tmp.GetPixel(x, y) ? "##" : "  ");
      }
      fprintf(stderr, "\n");
    }
    exit(1);
  }
}

template<class T>
void PatternTest() {
  T frame;
  frame.Clear();
  for (int x = 0; x < T::W; x++) {
    frame.SetPixel(x, sin(x * x / 200.0) * T::H / 3 + T::H/2, true);
  }
  ConvertAndCompare(frame);
  frame.Clear();
  for (int y = 0; y < T::H; y++) {
    for (int x = 0; x < T::W; x++) {
      frame.SetPixel(x, y, !!( ((x/4) ^ (y/4)) & 1 ));
    }
  }
  ConvertAndCompare(frame);
  frame.Clear();
  for (int y = 0; y < T::H; y++) {
    for (int x = 0; x < T::W; x++) {
      frame.SetPixel(x, y, (rand() & 0xff) < 20);
    }
  }
  ConvertAndCompare(frame);
};

template<int WIDTH, class col_t>
void PatternTest2() {
  fprintf(stderr, "H=%d Landscape\n", (int)(sizeof(col_t)*8));
  PatternTest<LandScapeFrame<WIDTH, col_t>>();
  fprintf(stderr, "Portrait\n");
  PatternTest<PortraitFrame<WIDTH, col_t>>();
}

template<int WIDTH>
void PatternTest3() {
  fprintf(stderr, "W=%d\n", WIDTH);
  PatternTest2<WIDTH, uint16_t>();
  PatternTest2<WIDTH, uint32_t>();
  PatternTest2<WIDTH, uint64_t>();
}

int main() {
  PatternTest3<64>();
  PatternTest3<128>();
  PatternTest3<128+64>();
}

