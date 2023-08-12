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

// 0b00000000 (0) = reserved
// 0b0NNNNNNN (1-123) = delta
// 0b01111100 (124) = copy 2
// 0b01111101 (125) = copy 4
// 0b01111110 (126) = copy 4x2
// 0b01111111 (127) = color is next 2 bytes
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
    int gd = gd_bits - 0x10;
    int ld = gd / 2 - 8;
    int rd = rd_bits + ld;
    int bd = bd_bits + ld;
    return (rd << 11) + (gd << 5) + bd;
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


#define PQOI_DECODE_ONE(GETC)					\
      GETC(AT_START);						\
      switch (byte) {						\
	case 0:							\
	case 1 ... 123:						\
	  pixel_ += deltas[byte];				\
	  break;						\
        case 124: /* copy 2 */					\
	  out[0] = out[-2];					\
	  out[1] = out[-1];					\
	  out += 2;						\
	  continue;						\
        case 125: /* copy 4 */					\
	  out[0] = out[-4];					\
	  out[1] = out[-3];					\
	  out[2] = out[-2];					\
	  out[3] = out[-1];					\
	  out += 4;						\
	  continue;						\
        case 126: /* copy 4x2 */				\
	  out[0] = out[-4];					\
	  out[1] = out[-3];					\
	  out[2] = out[-2];					\
	  out[3] = out[-1];					\
	  out[4] = out[-4];					\
	  out[5] = out[-3];					\
	  out[6] = out[-2];					\
	  out[7] = out[-1];					\
	  out += 8;						\
	  continue;						\
	case 127: { /* raw bytes */				\
	  GETC(AT_RAW1);					\
	  b2 = byte;						\
	  GETC(AT_RAW2);					\
	  pixel_ = (byte << 8) + b2;				\
	  break;						\
	}                                                       \
	case 159: *(out++) = pixel_;				\
	case 158: *(out++) = pixel_;				\
	case 157: *(out++) = pixel_;				\
	case 156: *(out++) = pixel_;				\
	case 155: *(out++) = pixel_;				\
	case 154: *(out++) = pixel_;				\
	case 153: *(out++) = pixel_;				\
	case 152: *(out++) = pixel_;				\
	case 151: *(out++) = pixel_;				\
	case 150: *(out++) = pixel_;				\
	case 149: *(out++) = pixel_;				\
	case 148: *(out++) = pixel_;				\
	case 147: *(out++) = pixel_;				\
	case 146: *(out++) = pixel_;				\
	case 145: *(out++) = pixel_;				\
	case 144: *(out++) = pixel_;				\
	case 143: *(out++) = pixel_;				\
	case 142: *(out++) = pixel_;				\
	case 141: *(out++) = pixel_;				\
	case 140: *(out++) = pixel_;				\
	case 139: *(out++) = pixel_;				\
	case 138: *(out++) = pixel_;				\
	case 137: *(out++) = pixel_;				\
	case 136: *(out++) = pixel_;				\
	case 135: *(out++) = pixel_;				\
	case 134: *(out++) = pixel_;				\
	case 133: *(out++) = pixel_;				\
	case 132: *(out++) = pixel_;				\
	case 131: *(out++) = pixel_;				\
	case 130: *(out++) = pixel_;				\
	case 129: *(out++) = pixel_;				\
	case 128: *(out++) = pixel_;				\
	  *(out++) = pixel_;                                    \
	  continue;						\
	case 160 ... 191: {					\
	  int gd_bits = byte - 160;				\
	  GETC(AT_LUMA);					\
	  pixel_ += lumaDiff(gd_bits, byte >> 4, byte & 0xf);	\
	  break;						\
	}							\
	case 192 ... 255:					\
	  *(out++) = pixel_ = stash_[byte - 192];		\
	  continue;						\
      }								\
      stash_[hashPixel(pixel_)] = pixel_;			\
      *(out++) = pixel_;



class PqoiDecoder : public PqoiBase {
public:
  const uint8_t* DecodeBlock(const uint8_t* in, const uint8_t* input_end, uint16_t* out, uint16_t* end) {
#define PQOI_SIMPLE_GETC(LABEL) byte = *(in++)    
    while (out < end) {
      uint8_t byte, b2;
      PQOI_DECODE_ONE( PQOI_SIMPLE_GETC );
    }
    return in;
  }
};

class PqoiGetcDecoder : public PqoiBase {
public:
  // Must return a character
  virtual uint8_t getc() = 0;

  // Read data until out is >= end.
  // At most 32 extra pixels will be returned.
  uint16_t* read(uint16_t* out, uint16_t* end) {
    while (out < end) {
      uint8_t byte, b2;
#define PQOI_GETC_GETC(LABEL) byte = getc()
      PQOI_DECODE_ONE( PQOI_GETC_GETC );
    }
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
#define PQOI_STREAMING_GETC(STATE) if (in >= input_end) { previous_byte = byte; state_=STATE; return out; } byte = *(in++)
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
	break;
      }

      case AT_START:
	break;
    }
    while (out < end) {
      PQOI_DECODE_ONE( PQOI_STREAMING_GETC );
    }
    state_ = AT_START;
    return out;
  }
};


template<size_t CHUNK_SIZE>
struct PqoiOutputChunk {
  static const size_t PAD = 32;
  static const size_t HISTORY = 4;
  // First chunk
  PqoiOutputChunk() : end_(begin()) {}

  explicit PqoiOutputChunk(PqoiOutputChunk* previous) {
    init_next_chunk(previous);
  }

  void init_next_chunk(PqoiOutputChunk* previous) {
    size_t pad = previous->end_ - previous->chunk_end();
    size_t history = 0;
    if (pad < 4) history = 4 - pad;
    size_t to_copy = history + pad;
    memcpy(begin() - history, previous->end_ - to_copy, to_copy * 2);
    end_ = begin() + history;
  }

  uint16_t* begin() const { return pixels + HISTORY; }
  uint16_t* chunk_end() const { return  begin() + CHUNK_SIZE; }
  uint16_t* end() const { return  std::min(end_, chunk_end()); }
  size_t size() const { return end() - begin(); }
  bool full() const { return size() >= CHUNK_SIZE; }
  uint16_t* data_end() const { return end_; }

  template<class PQOI_DECODER>
  bool fill(PQOI_DECODER* decoder, size_t pixels = CHUNK_SIZE) {
    if (size() >= pixels) return true;
    uint16_t read_end = std::min(chunk_end(), begin() + pixels);
    end_ = decoder->read(end_, read_end);
    return full();
  }
  
  uint16_t* end_;
  uint16_t pixels[HISTORY + CHUNK_SIZE + PAD];
};

struct PqoiLookupTable {
  constexpr PqoiLookupTable() : diff_index() {
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
    for (size_t i = 1; i < 124; i++) {
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
	op = (127 << 16) | ((pixel & 0xff) << 8) | (pixel >> 8);
      }
      // Check run and copy operations
#if 1
      if (pixels - begin >= 2 && end - pixels >= 2 &&
	  pixels[0] == pixels[-2] &&
	  pixels[1] == pixels[-1]) {
	l = 2;
	op = 124;
      }
      if (pixels - begin >= 4 && end - pixels >= 4 &&
	  pixels[0] == pixels[-4] &&
	  pixels[1] == pixels[-3] &&
	  pixels[2] == pixels[-2] &&
	  pixels[3] == pixels[-1]) {
	l = 4;
	op = 125;
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
	op = 126;
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
//	  fprintf(stderr, "%d %d %d     %x %x %x\n", in[0], in[1], in[2], out[0], out[1], out[2]);
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
//	  fprintf(stderr,"{%d %d %d} => %x\n", R, G, B, pixel);
	  if (channels == 4) {
	    int A = rgba[3];
	    // premultiply
	    A = (255 - A)  * 33 >> 8;
	    int Ainv = 32 - A;
	    R = (R * Ainv) >> 6;
	    G = (G * Ainv) >> 6;
	    B = (B * Ainv) >> 6;
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
	end = encoder.encodePixelsWithAlpha(data.pixels.data(), data.pixels.data() + pixels, data.alphas.data(), ret.data() + 16);
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
