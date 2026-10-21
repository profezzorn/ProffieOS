#ifndef DISPLAY_MONOFRAME_H
#define DISPLAY_MONOFRAME_H

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
#include "Aurebesh10Font.h"

#ifndef MAX_GLYPH_HEIGHT
#define MAX_GLYPH_HEIGHT 32
#endif

template<int Width, class col_t>
class MonoFrame {
public:
  static const int WIDTH = Width;
  static const int HEIGHT = sizeof(col_t) * 8;
  col_t frame_buffer_[WIDTH];

  void Clear() {
    memset(frame_buffer_, 0, WIDTH * HEIGHT / 8);
  }

  bool isClear() {
    for (int i = 0; i < WIDTH; i++) {
      if (frame_buffer_[i]) return false;
    }
    return true;
  }

  void Invert() {
    for (int i = 0; i < WIDTH; i++) {
      frame_buffer_[i] ^= (col_t)-1;
    }
  }

  void TransposeSquareInPlace(col_t* A) {
    int j, k; 
    col_t m, t; 

    m = 1;
    m = (m << ( sizeof(col_t) * 4)) - 1;
    for (j = sizeof(col_t)*4; j != 0; j = j >> 1, m = m ^ (m << j)) {
      for (k = 0; k < (int)(sizeof(col_t) * 8); k = (k + j + 1) & ~j) {
	t = (A[k] ^ (A[k+j] >> j)) & m; 
	A[k] = A[k] ^ t; 
	A[k+j] = A[k+j] ^ (t << j); 
      } 
    }
  }

  void FlipX() {
    for (int i = 0; i < WIDTH / 2; i++) {
      std::swap(frame_buffer_[i], frame_buffer_[WIDTH - 1 - i]);
    }
  }

  col_t ReverseBits(col_t word) {
    int j;
    col_t m = (1 << ( sizeof(col_t) * 4)) - 1;
    for (j = sizeof(col_t)*4; j != 0; j = j >> 1, m = m ^ (m << j)) {
      word = ((word >> j) & m) | ((word & m) << j);
    }
    return word;
  }
  
  void FlipY() {
    for (int w = 0; w < WIDTH; w++) {
      frame_buffer_[w] = ReverseBits(frame_buffer_[w]);
    }
  }

  col_t ReverseBytes(col_t word) {
    if (sizeof(col_t) == 2) return __builtin_bswap16(word);
    if (sizeof(col_t) == 4) return __builtin_bswap32(word);
    if (sizeof(col_t) == 8) return __builtin_bswap64(word);
    int j;
    col_t m = 1;
    m = (m << ( sizeof(col_t) * 4)) - 1;
    for (j = sizeof(col_t)*4; j != 4; j = j >> 1, m = m ^ (m << j)) {
      word = ((word >> j) & m) | ((word & m) << j);
    }
    return word;
  }
  
  void ConvertPortrait() {
    for (int w = 0; w < WIDTH; w++) {
      frame_buffer_[w] = ReverseBytes(frame_buffer_[w]);
    }
  }

  void ConvertLandscape(bool invert_y) {
    constexpr size_t wordsize = sizeof(col_t) * 8;
    uint32_t bits[(WIDTH + 31) / 32];
    for (uint32_t j = 0; j < NELEM(bits); j++) bits[j] = 0;
    for (int w = 0; w < WIDTH; w++) {
      if ((bits[w >> 5] >> (w & 31)) & 1) {
	// this word has already been moved
	continue;
      }
      size_t blocks_per_row = WIDTH / (sizeof(col_t) * 8);
#if 1
      int from = w;
      col_t tmp = frame_buffer_[from];
      do {
	tmp = ReverseBytes(tmp);
	int block_x = from % blocks_per_row;
	int row = from / blocks_per_row;
	if (!invert_y) row = wordsize - 1 - row;
	int dest = block_x * wordsize + row;
	std::swap(tmp, frame_buffer_[dest]);
	bits[dest >> 5] |= 1 << (dest & 31);
	from = dest;
      } while (from != w);
#endif      
    }
#if 1
    for(int w = 0; w < WIDTH; w += wordsize)
      TransposeSquareInPlace(frame_buffer_ + w);
#endif    
  }

  void SetPixel(int x, int y) {
    frame_buffer_[x] |= ((col_t)1) << y;
  }

  bool GetPixel(int x, int y) {
    return !!(frame_buffer_[x] & (((col_t)1) << y));
  }

  // y1 < y2
  void DrawVLine(int x, int y1, int y2) {
    frame_buffer_[x] |= (((col_t)-1) >> (HEIGHT - (y2 - y1))) << y1;
  }

  // x1 < x2
  void DrawHLine(int x1, int x2, int y) {
    col_t tmp = ((col_t)1) << y;
    for (int x = x1 ; x < x2; x++) frame_buffer_[x] |= tmp;
  }

  // x1 < x2, y1 < y2
  void DrawRect(int x1, int x2, int y1, int y2) {
    col_t tmp = (((col_t)-1) >> (HEIGHT - (y2 - y1))) << y1;
    for (int x = x1 ; x < x2; x++) frame_buffer_[x] |= tmp;
  }

  // x1 < x2, y1 < y2
  void ClearRect(int x1, int x2, int y1, int y2) {
    col_t tmp = ~(((col_t)-1) >> (HEIGHT - (y2 - y1))) << y1;
    for (int x = x1 ; x < x2; x++) frame_buffer_[x] &= tmp;
  }

  template<typename T>
  void Draw2(int begin, int end, col_t* pos, int shift, const T* data) {
    if (shift > 0) {
      for (int i = begin; i < end; i++) pos[i] |= ((col_t)data[i]) << shift;
    } else if (shift < 0) {
      for (int i = begin; i < end; i++) pos[i] |= ((col_t)data[i]) >> -shift;
    } else {
      for (int i = begin; i < end; i++) pos[i] |= data[i];
    }
  }

  void Draw(const Glyph& glyph, int x, int y, float scale = 1.0f) {
    if (scale == 1.0f) {
      x += glyph.xoffset;
      y += glyph.yoffset;
      int begin = std::max<int>(0, -x);
      int end = std::min<int>(glyph.columns, WIDTH - x);
      col_t *pos = frame_buffer_ + x;
      switch (glyph.column_size) {
        case 0:
          Draw2<uint8_t>(begin, end, pos, y, (const uint8_t*)glyph.data);
          break;
#if MAX_GLYPH_HEIGHT >= 16
      case 1:
        Draw2<uint16_t>(begin, end, pos, y, (const uint16_t*)glyph.data);
        break;
#endif	
#if MAX_GLYPH_HEIGHT >= 32
      case 3:
        Draw2<uint32_t>(begin, end, pos, y, (const uint32_t*)glyph.data);
        break;
#endif	
#if MAX_GLYPH_HEIGHT >= 64
      case 7:
        Draw2<uint64_t>(begin, end, pos, y, (const uint64_t*)glyph.data);
        break;
#endif	
      }
      return;
    }

    // Scaled rendering for custom display heights, e.g. 64x48 OLEDs.
    int glyph_height = 8;
    if (glyph.column_size == 1) glyph_height = 16;
    else if (glyph.column_size == 3) glyph_height = 32;
    else if (glyph.column_size == 7) glyph_height = 64;

    int offset_x = (int)floorf(glyph.xoffset * scale);
    int offset_y = (int)floorf(glyph.yoffset * scale);
    int width = (int)(glyph.columns * scale + 0.5f);
    int height = (int)(glyph_height * scale + 0.5f);

    for (int dx = 0; dx < width; dx++) {
      int sx = (int)(dx / scale);
      if (sx >= glyph.columns) sx = glyph.columns - 1;

      uint64_t bits = 0;
      switch (glyph.column_size) {
        case 0:
          bits = ((const uint8_t*)glyph.data)[sx];
          break;
        case 1:
          bits = ((const uint16_t*)glyph.data)[sx];
          break;
        case 3:
          bits = ((const uint32_t*)glyph.data)[sx];
          break;
        case 7:
          bits = ((const uint64_t*)glyph.data)[sx];
          break;
      }

      for (int dy = 0; dy < height; dy++) {
        int sy = (int)(dy / scale);
        if (sy >= glyph_height) sy = glyph_height - 1;

        if (bits & (1ULL << sy)) {
          int px = x + offset_x + dx;
          int py = y + offset_y + dy;

          if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            frame_buffer_[px] |= ((col_t)1 << py);
          }
        }
      }
    }
  }

  void DrawBatteryBar(const Glyph& bar, float percent, int y = 0) {
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
    int bars = floorf(percent* (0.5 + max_bars) / 100);
    for (int i = 0; i < bars; i++) {
      Draw(bar, pos, y);
      pos += bar.skip;
    }
  }

  void DrawText(const char* str,
                int x, int y,
                const Glyph* font,
                float scale = 1.0f,
                int line_height = 16) {
    while (*str) {
      if (*str == '\n') {
        x = 0;
        y += line_height;
      } else if (*str >= 0x20 && *str <= 0x7f) {
        const Glyph& glyph = font[*str - 0x20];
        int glyph_width = (int)(glyph.skip * scale + 0.5f);

        // Forced wrap: if this glyph won't fit on the current line,
        // move to the next line before drawing it.
        if (x > 0 && x + glyph_width > WIDTH) {
          x = 0;
          y += line_height;
        }

        Draw(glyph, x, y, scale);
        x += glyph_width;
      }
      str++;
    }
  }
};

#endif
