#ifndef PQOI_H
#define PQOI_H

#include <algorithm>
#include <stdint.h>
#include <memory.h>
#include <math.h>

#ifndef PQOI_LL_ONLY
#include <vector>
#include <stdio.h>
#endif

// QOI-inspired format for 16-bit RGB565 images and animations.

// 0b00000000 (0) = color is next 2 bytes
// 0b0NNNNNNN (1-124) = delta
// 0b01111100 (125) = copy 2
// 0b01111101 (126) = copy 4
// 0b01111110 (127) = copy 4x2
// 0b100NNNNN (128 ... 159) = run
// 0b101GGGGG 0bRRRRBBBB  (160 ... 191) = extended delta
// 0b11NNNNNN = (192..255) from stash

// Image files are stored with a simple header:
// PQOIXXXXYYYY (XXXX = width, YYYY = height, both as little-endian uint32_t)

namespace PQOI {

class PqoiBase {
public:
  uint16_t stash_[64];
  uint16_t pixel_;
  PqoiBase() { reset(); }
  static const uint16_t deltas[125];
  static uint32_t hashPixel(uint16_t pixel) {
    uint32_t tmp = pixel;
    uint32_t prime = 173976727;
    return (tmp * prime) >> 26;
  }
  static uint16_t lumaDiff(int gd_bits, int rd_bits, int bd_bits) {
#if 0
    static const uint16_t g[32] = {
     0x7DF0, 0x8611, 0x8631, 0x8E52, 0x8E72, 0x9693, 0x96B3, 0x9ED4,
     0x9EF4, 0xA715, 0xA735, 0xAF56, 0xAF76, 0xB797, 0xB7B7, 0xBFD8,
     0xBFF8, 0xC018, 0xC839, 0xC859, 0xD07A, 0xD09A, 0xD8BB, 0xD8DB,
     0xE0FC, 0xE11C, 0xE93D, 0xE95D, 0xF17E, 0xF19E, 0xF9BF, 0xF9DF,
    };
    return g[gd_bits] + (rd_bits << 11) + bd_bits;
#else
    int gd = gd_bits - 0x10;
    int ld = gd / 2 - 8;
    int rd = rd_bits + ld;
    int bd = bd_bits + ld;
    return (rd << 11) + (gd << 5) + bd;
#endif
  }
  static uint16_t lumaDiffRB(uint8_t byte) {
#if 1
    return ((byte >> 4) << 11) +  (byte & 0xf);
#else
    return ((byte << 7) | byte) & 0x780f;
#endif
  }
public:
  void reset() {
    memset(stash_, 0, sizeof(stash_));
    pixel_ = 0;
  }
};

#define PQOI_DELTA0(X) ((X) & 0xFFFF)
#define PQOI_DELTA1(X) PQOI_DELTA0(X), PQOI_DELTA0(X + 1), PQOI_DELTA0(X + 2), PQOI_DELTA0(X + 0xFFFE), PQOI_DELTA0(X + 0xFFFF)
#define PQOI_DELTA2(X) PQOI_DELTA1(X), PQOI_DELTA1(X + (1 << 5)),  PQOI_DELTA1(X + (2 << 5)),  PQOI_DELTA1(X + (0xFFFE << 5)),  PQOI_DELTA1(X + (0xFFFF << 5))
#define PQOI_DELTA3(X) PQOI_DELTA2(X), PQOI_DELTA2(X + (1 << 11)), PQOI_DELTA2(X + (2 << 11)), PQOI_DELTA2(X + (0xFFFE << 11)), PQOI_DELTA2(X + (0xFFFF << 11))
uint16_t const PqoiBase::deltas[125] = { PQOI_DELTA3(0) };

#ifdef PQOI_HISTOGRAM
uint64_t PQHIST[256] = {};
#define INCREMENT_PQ_HISTOGRAM(X) (PQHIST[(byte)]++)
#else
#define INCREMENT_PQ_HISTOGRAM(X) do { } while(0)
#endif

#define PQOI_DECODE_ONE(GETC)                                   \
      GETC(AT_START);                                           \
      INCREMENT_PQ_HISTOGRAM();                                 \
      switch (byte) {                                           \
        case 0: { /* raw bytes */                               \
          GETC(AT_RAW1);                                        \
          b2 = byte;                                            \
          GETC(AT_RAW2);                                        \
          pixel_ = (byte << 8) + b2;                            \
          break;                                                \
        }                                                       \
        case 1: pixel_ += 0x0001; break;                        \
        case 2: pixel_ += 0x0002; break;                        \
        case 3: pixel_ += 0xfffe; break;                        \
        case 4: pixel_ += 0xffff; break;                        \
        case 5: pixel_ += 0x0020; break;                        \
        case 6: pixel_ += 0x0021; break;                        \
        case 7: pixel_ += 0x0022; break;                        \
        case 8: pixel_ += 0x001e; break;                        \
        case 9: pixel_ += 0x001f; break;                        \
        case 10: pixel_ += 0x0040; break;                       \
        case 11: pixel_ += 0x0041; break;                       \
        case 12: pixel_ += 0x0042; break;                       \
        case 13: pixel_ += 0x003e; break;                       \
        case 14: pixel_ += 0x003f; break;                       \
        case 15: pixel_ += 0xffc0; break;                       \
        case 16: pixel_ += 0xffc1; break;                       \
        case 17: pixel_ += 0xffc2; break;                       \
        case 18: pixel_ += 0xffbe; break;                       \
        case 19: pixel_ += 0xffbf; break;                       \
        case 20: pixel_ += 0xffe0; break;                       \
        case 21: pixel_ += 0xffe1; break;                       \
        case 22: pixel_ += 0xffe2; break;                       \
        case 23: pixel_ += 0xffde; break;                       \
        case 24: pixel_ += 0xffdf; break;                       \
        case 25: pixel_ += 0x0800; break;                       \
        case 26: pixel_ += 0x0801; break;                       \
        case 27: pixel_ += 0x0802; break;                       \
        case 28: pixel_ += 0x07fe; break;                       \
        case 29: pixel_ += 0x07ff; break;                       \
        case 30: pixel_ += 0x0820; break;                       \
        case 31: pixel_ += 0x0821; break;                       \
        case 32: pixel_ += 0x0822; break;                       \
        case 33: pixel_ += 0x081e; break;                       \
        case 34: pixel_ += 0x081f; break;                       \
        case 35: pixel_ += 0x0840; break;                       \
        case 36: pixel_ += 0x0841; break;                       \
        case 37: pixel_ += 0x0842; break;                       \
        case 38: pixel_ += 0x083e; break;                       \
        case 39: pixel_ += 0x083f; break;                       \
        case 40: pixel_ += 0x07c0; break;                       \
        case 41: pixel_ += 0x07c1; break;                       \
        case 42: pixel_ += 0x07c2; break;                       \
        case 43: pixel_ += 0x07be; break;                       \
        case 44: pixel_ += 0x07bf; break;                       \
        case 45: pixel_ += 0x07e0; break;                       \
        case 46: pixel_ += 0x07e1; break;                       \
        case 47: pixel_ += 0x07e2; break;                       \
        case 48: pixel_ += 0x07de; break;                       \
        case 49: pixel_ += 0x07df; break;                       \
        case 50: pixel_ += 0x1000; break;                       \
        case 51: pixel_ += 0x1001; break;                       \
        case 52: pixel_ += 0x1002; break;                       \
        case 53: pixel_ += 0x0ffe; break;                       \
        case 54: pixel_ += 0x0fff; break;                       \
        case 55: pixel_ += 0x1020; break;                       \
        case 56: pixel_ += 0x1021; break;                       \
        case 57: pixel_ += 0x1022; break;                       \
        case 58: pixel_ += 0x101e; break;                       \
        case 59: pixel_ += 0x101f; break;                       \
        case 60: pixel_ += 0x1040; break;                       \
        case 61: pixel_ += 0x1041; break;                       \
        case 62: pixel_ += 0x1042; break;                       \
        case 63: pixel_ += 0x103e; break;                       \
        case 64: pixel_ += 0x103f; break;                       \
        case 65: pixel_ += 0x0fc0; break;                       \
        case 66: pixel_ += 0x0fc1; break;                       \
        case 67: pixel_ += 0x0fc2; break;                       \
        case 68: pixel_ += 0x0fbe; break;                       \
        case 69: pixel_ += 0x0fbf; break;                       \
        case 70: pixel_ += 0x0fe0; break;                       \
        case 71: pixel_ += 0x0fe1; break;                       \
        case 72: pixel_ += 0x0fe2; break;                       \
        case 73: pixel_ += 0x0fde; break;                       \
        case 74: pixel_ += 0x0fdf; break;                       \
        case 75: pixel_ += 0xf000; break;                       \
        case 76: pixel_ += 0xf001; break;                       \
        case 77: pixel_ += 0xf002; break;                       \
        case 78: pixel_ += 0xeffe; break;                       \
        case 79: pixel_ += 0xefff; break;                       \
        case 80: pixel_ += 0xf020; break;                       \
        case 81: pixel_ += 0xf021; break;                       \
        case 82: pixel_ += 0xf022; break;                       \
        case 83: pixel_ += 0xf01e; break;                       \
        case 84: pixel_ += 0xf01f; break;                       \
        case 85: pixel_ += 0xf040; break;                       \
        case 86: pixel_ += 0xf041; break;                       \
        case 87: pixel_ += 0xf042; break;                       \
        case 88: pixel_ += 0xf03e; break;                       \
        case 89: pixel_ += 0xf03f; break;                       \
        case 90: pixel_ += 0xefc0; break;                       \
        case 91: pixel_ += 0xefc1; break;                       \
        case 92: pixel_ += 0xefc2; break;                       \
        case 93: pixel_ += 0xefbe; break;                       \
        case 94: pixel_ += 0xefbf; break;                       \
        case 95: pixel_ += 0xefe0; break;                       \
        case 96: pixel_ += 0xefe1; break;                       \
        case 97: pixel_ += 0xefe2; break;                       \
        case 98: pixel_ += 0xefde; break;                       \
        case 99: pixel_ += 0xefdf; break;                       \
        case 100: pixel_ += 0xf800; break;                      \
        case 101: pixel_ += 0xf801; break;                      \
        case 102: pixel_ += 0xf802; break;                      \
        case 103: pixel_ += 0xf7fe; break;                      \
        case 104: pixel_ += 0xf7ff; break;                      \
        case 105: pixel_ += 0xf820; break;                      \
        case 106: pixel_ += 0xf821; break;                      \
        case 107: pixel_ += 0xf822; break;                      \
        case 108: pixel_ += 0xf81e; break;                      \
        case 109: pixel_ += 0xf81f; break;                      \
        case 110: pixel_ += 0xf840; break;                      \
        case 111: pixel_ += 0xf841; break;                      \
        case 112: pixel_ += 0xf842; break;                      \
        case 113: pixel_ += 0xf83e; break;                      \
        case 114: pixel_ += 0xf83f; break;                      \
        case 115: pixel_ += 0xf7c0; break;                      \
        case 116: pixel_ += 0xf7c1; break;                      \
        case 117: pixel_ += 0xf7c2; break;                      \
        case 118: pixel_ += 0xf7be; break;                      \
        case 119: pixel_ += 0xf7bf; break;                      \
        case 120: pixel_ += 0xf7e0; break;                      \
        case 121: pixel_ += 0xf7e1; break;                      \
        case 122: pixel_ += 0xf7e2; break;                      \
        case 123: pixel_ += 0xf7de; break;                      \
        case 124: pixel_ += 0xf7df; break;                      \
        case 125: /* copy 2 */                                  \
          out[0] = out[-2];                                     \
          out[1] = out[-1];                                     \
          out += 2;                                             \
          continue;                                             \
        case 126: /* copy 4 */                                  \
          out[0] = out[-4];                                     \
          out[1] = out[-3];                                     \
          out[2] = out[-2];                                     \
          out[3] = out[-1];                                     \
          out += 4;                                             \
          continue;                                             \
        case 127: /* copy 4x2 */                                \
          out[0] = out[-4];                                     \
          out[1] = out[-3];                                     \
          out[2] = out[-2];                                     \
          out[3] = out[-1];                                     \
          out[4] = out[-4];                                     \
          out[5] = out[-3];                                     \
          out[6] = out[-2];                                     \
          out[7] = out[-1];                                     \
          out += 8;                                             \
          continue;                                             \
        case 159: *(out++) = pixel_;                            \
        case 158: *(out++) = pixel_;                            \
        case 157: *(out++) = pixel_;                            \
        case 156: *(out++) = pixel_;                            \
        case 155: *(out++) = pixel_;                            \
        case 154: *(out++) = pixel_;                            \
        case 153: *(out++) = pixel_;                            \
        case 152: *(out++) = pixel_;                            \
        case 151: *(out++) = pixel_;                            \
        case 150: *(out++) = pixel_;                            \
        case 149: *(out++) = pixel_;                            \
        case 148: *(out++) = pixel_;                            \
        case 147: *(out++) = pixel_;                            \
        case 146: *(out++) = pixel_;                            \
        case 145: *(out++) = pixel_;                            \
        case 144: *(out++) = pixel_;                            \
        case 143: *(out++) = pixel_;                            \
        case 142: *(out++) = pixel_;                            \
        case 141: *(out++) = pixel_;                            \
        case 140: *(out++) = pixel_;                            \
        case 139: *(out++) = pixel_;                            \
        case 138: *(out++) = pixel_;                            \
        case 137: *(out++) = pixel_;                            \
        case 136: *(out++) = pixel_;                            \
        case 135: *(out++) = pixel_;                            \
        case 134: *(out++) = pixel_;                            \
        case 133: *(out++) = pixel_;                            \
        case 132: *(out++) = pixel_;                            \
        case 131: *(out++) = pixel_;                            \
        case 130: *(out++) = pixel_;                            \
        case 129: *(out++) = pixel_;                            \
        case 128: *(out++) = pixel_;                            \
          *(out++) = pixel_;                                    \
          continue;                                             \
        case 160 ... 191: {                                     \
          int gd_bits = byte - 160;                             \
          GETC(AT_LUMA);                                        \
          pixel_ += lumaDiff(gd_bits, byte >> 4, byte & 0xf);   \
          break;                                                \
        }                                                       \
        case 192 ... 255:                                       \
          *(out++) = pixel_ = stash_[byte - 192];               \
          continue;                                             \
      }                                                         \
      stash_[hashPixel(pixel_)] = pixel_;                       \
      *(out++) = pixel_;


class PqoiDecoder : public PqoiBase {
public:
  const uint8_t* DecodeBlock(const uint8_t* in, const uint8_t* input_end, uint16_t* out, uint16_t* end) {
    uint16_t pixel_ = this->pixel_;
#define PQOI_SIMPLE_GETC(LABEL) byte = *(in++)
    while (out < end) {
      uint8_t byte, b2;
      PQOI_DECODE_ONE( PQOI_SIMPLE_GETC );
    }
    this->pixel_ = pixel_;
    return in;
  }
};

class PqoiGetcDecoder : public PqoiBase {
public:
  // Must return a character
  virtual uint8_t getc() = 0;

  // Read data until out is >= end.
  // At most 32 extra pixels will be returned.
  __attribute__((optimize("Ofast"))) uint16_t* read(uint16_t* out, uint16_t* end) {
    uint16_t pixel_ = this->pixel_;
    while (out < end) {
      uint8_t byte, b2;
#define PQOI_GETC_GETC(LABEL) byte = getc()
      PQOI_DECODE_ONE( PQOI_GETC_GETC );
    }
    this->pixel_ = pixel_;
    return out;
  }
};

class PqoiStreamingDecoder : public PqoiBase {
public:
  enum State {
    AT_START,
    AT_RAW1,
    AT_RAW2,
    AT_LUMA,
  };
  State state_ = AT_START;;
  uint8_t previous_byte = 0;
  const uint8_t* in;
  const uint8_t* input_end;

  bool input_done() const { return in >= input_end; }
  void set_input(const uint8_t* input, const uint8_t* end) {
    in = input;
    input_end = end;
  }

  // Read data until out is >= end.
  // At most 32 extra pixels will be returned.
  uint16_t* read(uint16_t* out, uint16_t* end) {
    uint8_t byte = previous_byte;
    uint8_t b2;
    uint16_t pixel_ = this->pixel_;
    const uint8_t* in = this->in;
    const uint8_t* input_end = this->input_end;

#define PQOI_STREAMING_GETC(STATE) if (in >= input_end) { previous_byte = byte; state_=STATE; this->pixel_ = pixel_; this->in=in; return out; } byte = *(in++)
    switch (state_) {
      case AT_RAW1:
        PQOI_STREAMING_GETC(AT_RAW1);
        /* fall through */
      case AT_RAW2:
        b2 = byte;
        PQOI_STREAMING_GETC(AT_RAW2);
        pixel_ = (byte << 8) + b2;
        stash_[hashPixel(pixel_)] = pixel_;
        *(out++) = pixel_;
        break;

      case AT_LUMA: {
        int gd_bits = byte - 160;
        PQOI_STREAMING_GETC(AT_LUMA);
        pixel_ += lumaDiff(gd_bits, byte >> 4, byte & 0xf);
        stash_[hashPixel(pixel_)] = pixel_;
        *(out++) = pixel_;
        break;
      }

      case AT_START:
        break;
    }

    while (out < end) {
      PQOI_DECODE_ONE( PQOI_STREAMING_GETC );
    }
    this->pixel_ = pixel_;
    this->in = in;
    state_ = AT_START;
    return out;
  }
};

template<size_t CHUNK_SIZE>
struct PqoiOutputChunk {
  static const size_t PAD = 32;
  static const size_t HISTORY = 4;
  // First chunk
  PqoiOutputChunk() {
    init();
  }

  explicit PqoiOutputChunk(PqoiOutputChunk* previous) {
    init_next_chunk(previous);
  }

  void init() {
    end_ = begin();
  }

  void init(int left) {
    end_ = begin() + left;
  }

  void init_next_chunk(PqoiOutputChunk* previous) {
    // Previous must be full()
    size_t pad = previous->end_ - previous->chunk_end();
    size_t history = 0;
    if (pad < 4) history = 4 - pad;
    size_t to_copy = history + pad;
    memcpy(begin() - history, previous->end_ - to_copy, to_copy * 2);
    end_ = begin() + pad;
  }

  void init_next_chunk(PqoiOutputChunk* previous, int left, int pixels) {
    // Previous must be full()
    size_t pad = previous->end_ - (previous->begin() + left + pixels);
    size_t history = 0;
    if (pad < 4) history = 4 - pad;
    size_t to_copy = history + pad;
    memcpy(begin() + left - history, previous->end_ - to_copy, to_copy * 2);
    end_ = begin() + left + pad;
  }

  uint16_t* begin() { return pixels + HISTORY; }
  uint16_t* chunk_end() { return  begin() + CHUNK_SIZE; }
  uint16_t* end() { return  std::min(end_, chunk_end()); }
  size_t size() { return end() - begin(); }
  bool full() { return size() >= CHUNK_SIZE; }
  bool full(int left, int pixels) { return size() >= pixels + left; }
  uint16_t* data_end() { return end_; }

  template<class PQOI_DECODER>
  bool fill(PQOI_DECODER* decoder, size_t pixels = CHUNK_SIZE) {
    if (size() >= pixels) return true;
    uint16_t* read_end = std::min(chunk_end(), begin() + pixels);
    end_ = decoder->read(end_, read_end);
    return size() >= pixels;
  }

  template<class PQOI_DECODER>
  bool fill(PQOI_DECODER* decoder, size_t left, size_t pixels) {
    if (size() >= pixels + left) return true;
    uint16_t* read_end = std::min(chunk_end(), begin() + pixels + left);
    end_ = decoder->read(end_, read_end);
    return size() >= pixels + left;
  }

  void zero() {
    memset(begin(), 0, CHUNK_SIZE * 2);
    end_ = chunk_end();
  }
  void zero(size_t left,size_t pixels) {
    memset(begin() + left, 0, pixels * 2);
    end_ = begin() + left + pixels;
  }
  void zero_margins(size_t left, size_t pixels) {
    if (left) memset(begin(), 0, left * 2);
    if (left + pixels < CHUNK_SIZE) {
      memset(begin() + left + pixels, 0, (CHUNK_SIZE - left - pixels) * 2);
    }
    end_ = chunk_end();
  }

  uint16_t* end_;
  uint16_t pixels[HISTORY + CHUNK_SIZE + PAD];
};

struct PqoiLookupTable {
  PqoiLookupTable() {
    for (size_t i = 0; i < 0x10000; i++) {
      diff_index[i] = 0;
    }
    for (int gd_bits = 0; gd_bits < 32; gd_bits++) {
      for (int rd_bits = 0; rd_bits < 16; rd_bits++) {
        for (int bd_bits = 0; bd_bits < 16; bd_bits++) {
          uint16_t output = ((160 + gd_bits) << 8) + (rd_bits << 4) + bd_bits;
          diff_index[PqoiBase::lumaDiff(gd_bits, rd_bits, bd_bits)] = output;
        }
      }
    }
    for (size_t i = 1; i < 125; i++) {
      diff_index[PqoiBase::deltas[i]] = i;
    }
  }
  uint16_t diff_index[0x10000];
};

class PqoiEncoder : public PqoiBase {
public:
  PqoiEncoder() {
    static PqoiLookupTable lookupTable;
    diff_index = lookupTable.diff_index;
  }

  uint8_t* encodePixels(const uint16_t* begin, const uint16_t* end, uint8_t *out) {
    for (const uint16_t* pixels = begin; pixels < end; pixels++) {
      uint16_t pixel = *pixels;
      uint16_t diff = pixel - pixel_;
      int op = diff_index[diff];
      int h = hashPixel(pixel);
      int l = 1;
      if (stash_[h] == pixel) {
        op = h + 192;
      } else if (op == 0) {
        op = (0x100 << 16) | ((pixel & 0xff) << 8) | (pixel >> 8);
      }
      // Check run and copy operations
#if 1
      if (pixels - begin >= 2 && end - pixels >= 2 &&
          pixels[0] == pixels[-2] &&
          pixels[1] == pixels[-1]) {
        l = 2;
        op = 125;
      }
      if (pixels - begin >= 4 && end - pixels >= 4 &&
          pixels[0] == pixels[-4] &&
          pixels[1] == pixels[-3] &&
          pixels[2] == pixels[-2] &&
          pixels[3] == pixels[-1]) {
        l = 4;
        op = 126;
      }
      if (pixels - begin >= 8 && end - pixels >= 8 &&
          pixels[0] == pixels[-4] &&
          pixels[1] == pixels[-3] &&
          pixels[2] == pixels[-2] &&
          pixels[3] == pixels[-1] &&
          pixels[4] == pixels[-4] &&
          pixels[5] == pixels[-3] &&
          pixels[6] == pixels[-2] &&
          pixels[7] == pixels[-1]) {
        l = 8;
        op = 127;
      }
#endif
      if (pixel == pixel_) {
        int run = 0;
        while (run < 33 && pixels+run < end && pixels[run] == pixel_) run++;
        if (run > l) {
          op = run + 126;
          l = run;
        }
      }
      if (op > 65535) *(out++) = op >> 16;
      if (op > 255) *(out++) = op >> 8;
      *(out++) = op;
      pixels += l - 1;
      if (l == 1) {
        pixel_ = pixel;
        stash_[h] = pixel;
      }
    }
    return out;
  }
private:
  const uint16_t *diff_index;
};


// Alpha format:
// 0x00NNNNNN = run of N+1 transparent pixels
// 0x01NNNNNN = N+1 pixels with alpha values
// 0x10NNNNNN = N+1 pixels with *same* alpha value
// 0x11NNNNNN = run of N+1 opaque pixels
class AlphaDecoder : public PqoiDecoder {
public:
  static void alphaBlend(uint16_t& out, uint16_t in, uint8_t alpha) {
    // This transform should already have been done on the alpha values.
    // alpha = ((255 - alpha) * 33) >> 8;
    uint32_t tmp = (out * 0x10001) & 0x7E0F81F;
    tmp = ((tmp * alpha) >> 5) & 0x7EF81F;
    out = tmp + (tmp >> 16) + in;
  }
  void Apply(const uint8_t* in, const uint8_t* input_end, uint16_t* out, uint16_t* end) {
    while (out < end) {
      uint8_t byte = *(in++);
      uint8_t N = (byte & 0x3F) + 1;
      uint16_t tmp[64];
      switch (byte >> 6) {
        case 0: break;
        case 1: {
          in = DecodeBlock(in, input_end, tmp, tmp + N);
          for (int i = 0; i < N; i++) alphaBlend(out[i], tmp[i], *(in++));
          break;
        }
        case 2: {
          in = DecodeBlock(in, input_end, tmp, tmp + N);
          uint8_t alpha = *(in++);
          for (int i = 0; i < N; i++) alphaBlend(out[i], tmp[i], alpha);
          break;
        }
        case 3:
          in = DecodeBlock(in, input_end, out, out + N);
          break;
      }
      out += N;
    }
  }

  void DecodeBlockWithAlphas(const uint8_t* in, const uint8_t* input_end, uint16_t* out, uint16_t* end, uint8_t* alphaout) {
    while (out < end) {
      uint8_t byte = *(in++);
      uint8_t N = (byte & 0x3F) + 1;
      switch (byte >> 6) {
        case 0:
          for (int i = 0; i < N; i++) *(out++) = 0;
          memset(alphaout, 32, N);
          alphaout += N;
          break;
        case 1: {
          in = DecodeBlock(in, input_end, out, out + N);
          memcpy(alphaout, in, N);
          in += N;
          alphaout += N;
          out += N;
          break;
        }
        case 2: {
          in = DecodeBlock(in, input_end, out, out + N);
          memset(alphaout, *(in++), N);
          alphaout += N;
          out += N;
          break;
        }
        case 3:
          in = DecodeBlock(in, input_end, out, out + N);
//        fprintf(stderr, "%d %d %d     %x %x %x\n", in[0], in[1], in[2], out[0], out[1], out[2]);
          memset(alphaout, 0, N);
          alphaout += N;
          out += N;
          break;
      }
    }
  }
};

class StreamingAlphaDecoder : public PqoiStreamingDecoder {
  uint16_t tmp[64];
  uint16_t *read_pos, *read_until;
  uint8_t byte;
  int N, i;
  int state_ = 0;
#define PQOI_ALPHA_YIELD() do { state_ = __LINE__; return out; case __LINE__: break; } while(0)
public:
  // Decode alpha until out >= end, or input runs out.
  uint16_t* Apply(uint16_t* out, uint16_t* end) {
    switch (state_) {
    case 0:
      while (out < end) {
        while (input_done()) PQOI_ALPHA_YIELD();
        byte = *(in++);
        N = (byte & 0x3F) + 1;
        if ((byte >> 6) == 0) {
          // Do nothing.
        } else if ((byte >> 6) == 3) {
          // Read directly into output space.
          read_until = out + N;
          while (true) {
            out = read(out, read_until);
            if (out >= read_until) break;
            PQOI_ALPHA_YIELD();
          }
          continue;
        } else { // 1 or 2
          // Read into temporary space and alpha-blend.
          read_until = tmp + N;
          read_pos = tmp;
          while (true) {
            read_pos = read(read_pos, read_until);
            if (read_pos >= read_until) break;
            PQOI_ALPHA_YIELD();
          }
          if ((byte >> 6) == 1) {
            for (i = 0; i < N; i++) {
              while (input_done()) PQOI_ALPHA_YIELD();
              AlphaDecoder::alphaBlend(out[i], tmp[i], *(in++));
            }
          } else { // 2
            while (input_done()) PQOI_ALPHA_YIELD();
            uint8_t alpha = *(in++);
            for (i = 0; i < N; i++) AlphaDecoder::alphaBlend(out[i], tmp[i], alpha);
          }
        }
        out += N;
      }
    }
    state_ = 0;
    return out;
  }
};

class AlphaEncoder : public PqoiEncoder {
public:
  uint8_t getAlpha(const uint8_t* alphas) {
    // return  ((255 - *alphas) * 33) >> 8;
    return *alphas;
  }

  static uint16_t premultiply(uint16_t pixel, uint8_t alpha) {
    uint16_t x = 0xFFFF;
    AlphaDecoder::alphaBlend(x, 0, alpha);
    int r = 32 - (x >> 11);
    int g = 64 - ((x >> 5) & 0x3f);
    int b = 32 - (x & 0x1f);
    int R = pixel >> 11;
    int G = (pixel >> 5) & 0x3f;
    int B = pixel & 0x1f;
    r = r * R / 32;
    g = g * G / 64;
    b = b * B / 32;
    return (r << 11) + (g << 5) + b;
  }

  // alphas should be 0-32 (0=opaque, 32=transparent) and pixel values should be pre-multiplied.
  uint8_t* encodePixelsWithAlpha(const uint16_t* begin, const uint16_t* end, const uint8_t* alphas, uint8_t *out) {
    while (begin < end) {
      int run = 1;
      bool raw = false;
      uint8_t alpha = getAlpha(alphas);
      while (run < 64 && end - begin > run) {
        if (getAlpha(alphas + run) != alpha) {
          break;
        }
        run++;
      }
      if (run < 2) {
        run = 1;
        raw = true;

        while (run < 64 && end - begin > run + 1) {
          if (getAlpha(alphas + run + 1) == getAlpha(alphas + run) &&
              getAlpha(alphas + run + 2) == getAlpha(alphas + run)) {
            break;
          }
          run++;
        }
      }

      if (raw) {
        *(out++) = 0b01000000 + (run -1);
        out = encodePixels(begin, begin + run, out);
        for (int i = 0; i < run; i++) {
          *(out++) = getAlpha(alphas + i);
        }
      } else {
        switch (alpha) {
          case 32: // transparent
            *(out++) = run -1;
            break;
          case 0:   // opaque
            *(out++) = 0b11000000 + (run -1);
            out = encodePixels(begin, begin + run, out);
            break;
          default:
            *(out++) = 0b10000000 + (run -1);
            out = encodePixels(begin, begin + run, out);
            *(out++) = alpha;
        }
      }
      begin += run;
      alphas += run;
    }
    return out;
  }
};

#ifndef PQOI_LL_ONLY
  class EasyPqoiEncoder  {
  public:
    EasyPqoiEncoder() {
      mkditherMatrix(0,0,1);
    }
    int quantize(uint8_t value, int steps, int x, int y) {
      float V = value * steps / 255.0;
      V += (ditherMatrix[x & 3][y & 3] - 7.5)/ 16.0;
      int ret = floor(V);
      if (ret < 0) return 0;
      if (ret >= steps) return steps - 1;
      return ret;
    }
    struct RGB565Data {
      int w;
      int h;
      std::vector<uint16_t> pixels;
      std::vector<uint8_t> alphas;

      bool operator==(const RGB565Data& other) const {
        if (other.w != w) return false;
        if (other.h != h) return false;
        if (other.pixels.size() != pixels.size()) return false;
        if (other.alphas.size() != alphas.size()) return false;
        if (memcmp(other.pixels.data(), pixels.data(), pixels.size() * sizeof(uint16_t))) return false;
        if (memcmp(other.alphas.data(), alphas.data(), alphas.size() * sizeof(uint8_t))) return false;
        return true;
      }

      bool diff(const RGB565Data& other) const {
        if (other.w != w) {
          fprintf(stderr, "Width differs, %d != %d\n", other.w, w);
          return false;
        }
        if (other.h != h) {
          fprintf(stderr, "Height differs, %d != %d\n", other.h, h);
          return false;
        }
        if (pixels.size() != other.pixels.size()) {
          fprintf(stderr, "Wrong number of pixels %d != %d\n", (int)pixels.size(), (int)other.pixels.size());
          return false;
        }
        if (alphas.size() != other.alphas.size()) {
          fprintf(stderr, "Wrong number of alphas %d != %d\n", (int)alphas.size(), (int)other.alphas.size());
          return false;
        }
        int err = 0;
        for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
            if (pixels[y * w + x] != other.pixels[y * w + x]) {
              fprintf(stderr, "Pixel at %d, %d differs, %x != %x pp = %d w = %d h = %d\n", x, y, pixels[y * w + x], other.pixels[y * w + x], y * w + x, w, h);
              if (++err > 20) return false;
            }
            if (alphas.size()) {
              if (alphas[y * w + x] != other.alphas[y * w + x]) {
                fprintf(stderr, "Alpha at %d, %d differs, %d != %d\n", x, y, alphas[y * w + x], other.alphas[y * w + x]);
                if (++err > 20) return false;
              }
            }
          }
        }
        return err == 0;
      }
    };

    RGB565Data quantize(const uint8_t* rgba, int w, int h, int channels) {
      RGB565Data ret;
      ret.w = w;
      ret.h = h;
      bool opaque = true;
      for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
          int R = rgba[0];
          int G = rgba[1];
          int B = rgba[2];
//        fprintf(stderr,"{%d %d %d} => %x\n", R, G, B, pixel);
          if (channels == 4) {
            int A = rgba[3];
            // premultiply
            A = (255 - A)  * 33 >> 8;
            int Ainv = 32 - A;
            R = (R * Ainv) >> 5;
            G = (G * Ainv) >> 5;
            B = (B * Ainv) >> 5;
            ret.alphas.push_back(A);
            if (A) opaque = false;
          }
          uint16_t pixel = (quantize(R, 32, x, y) << 11) | (quantize(G, 64, x, y) << 5) | quantize(B, 32, x, y);
          ret.pixels.push_back(pixel);
          rgba += channels;
        }
      }
      // Pictures with entirely opaque alpha channels gets demoted to 3-channel pictures.
      if (opaque) ret.alphas.clear();
      return ret;
    }

    void AddNumber(uint32_t n, uint8_t* out) {
      for (int i = 0; i < 4; i++) {
        *(out++) = n & 0xff;
        n >>= 8;
      }
    }

    void AddHeader(const char magic[4],
                   uint32_t length,
                   uint32_t a,
                   uint32_t b,
                   uint8_t* out) {
      out[0] = magic[0];
      out[1] = magic[1];
      out[2] = magic[2];
      out[3] = magic[3];
      AddNumber(length, out + 4);
      AddNumber(a, out + 8);
      AddNumber(b, out + 12);
    }

    std::vector<uint8_t> encode(const RGB565Data& data) {
      size_t pixels = data.w * data.h;
      std::vector<uint8_t> ret(pixels * 5 + 16);
      uint8_t* end;
      const char* magic;
      if (data.alphas.size() == 0) {
        // Add header....
        PqoiEncoder encoder;
        magic = "PQOI";
        end = encoder.encodePixels(data.pixels.data(), data.pixels.data() + pixels, ret.data() + 16);
      } else {
        // Add header....
        AlphaEncoder encoder;
        magic = "PQOA";
        end = ret.data() + 16;
        // Alpha data must be encoded one row at a time, or per-row decoding becomes too complicated
        for (int row = 0; row < data.h; row++) {
          const uint16_t *pixel_row = data.pixels.data() + data.w * row;
          const uint8_t *alpha_row = data.alphas.data() + data.w * row;
          end = encoder.encodePixelsWithAlpha(pixel_row, pixel_row + data.w, alpha_row, end);
        }
      }
      size_t size = end - ret.data();
      ret.resize(size);
      AddHeader(magic, size - 8, data.w, data.h, ret.data());
      return ret;
    }

    uint32_t get32(const uint8_t* t) {
      return t[0] + (t[1] << 8) + (t[2] << 16) + (t[3] << 24);
    }

    static constexpr uint32_t onecc(char c, int shift) {
      return ((uint32_t)c) << shift;
    }
    static constexpr uint32_t fourcc(char const p[5] ) {
      return onecc(p[0], 0) + onecc(p[1], 8) + onecc(p[2], 16) + onecc(p[3], 24);
    }
    RGB565Data decode(const std::vector<uint8_t>& data) {
      RGB565Data ret;
      switch (get32(data.data())) {
        case fourcc("PQOI"): {
          ret.w = get32(data.data() + 8);
          ret.h = get32(data.data() + 12);
          ret.pixels.resize(ret.w * ret.h);
          PqoiDecoder decoder;
          decoder.DecodeBlock(data.data()+16, data.data()+data.size(), ret.pixels.data(), ret.pixels.data() + ret.w*ret.h);
          break;
        }
        case fourcc("PQOA"): {
          ret.w = get32(data.data() + 8);
          ret.h = get32(data.data() + 12);
          ret.pixels.resize(ret.w * ret.h);
          ret.alphas.resize(ret.w * ret.h);
          AlphaDecoder decoder;
          decoder.DecodeBlockWithAlphas(data.data()+16, data.data()+data.size(),
                                        ret.pixels.data(), ret.pixels.data() + ret.w*ret.h,
                                        ret.alphas.data());
          break;
        }
      }

      return ret;
    }
  private:
    void mkditherMatrix(int x, int y, int step) {
      if (x >= 4) return;
      if (y >= 4) return;
      if (step >= 4) {
        ditherMatrix[x][y] = dithertmp++;
        return;
      }
      mkditherMatrix(x, y, step * 2);
      mkditherMatrix(x + step, y + step, step * 2);
      mkditherMatrix(x, y + step, step * 2);
      mkditherMatrix(x + step, y, step * 2);
    }
    int ditherMatrix[4][4]; // 0..15
    int dithertmp = 0;
  };
#endif

}  // namespace PQOI

#endif
