/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <vector>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <memory.h>

#include "common.h"
#include "rle.h"

class Color8 {
  public:
  constexpr Color8() : r(0), g(0), b(0) {}
  constexpr Color8(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}

  uint8_t r, g, b;
};

class Color16 {
  public:
  constexpr Color16() : r(0), g(0), b(0) {}
  constexpr Color16(const Color8& c) : r(c.r * 0x101), g(c.g * 0x101), b(c.b * 0x101) {}
  constexpr Color16(uint16_t r_, uint16_t g_, uint16_t b_) : r(r_), g(g_), b(b_) {}

  uint16_t r, g, b;
};

inline Color16 operator*(const Color8& c, uint16_t x) {
  return Color16(c.r * x, c.g * x, c.b * x);
}


#include IMAGE_FILE

#define NELEM(X) (sizeof(X)/sizeof((X)[0]))

int main() {
  TooDee<RGB<float>> image;
  image.resize(NELEM(imageoffsets), POV_DATA_HEIGHT);
  
  for (int col = 0; col < NELEM(imageoffsets); col++) {
#ifdef POV_RGB
#define MULT 3
#else
#define MULT 1
#endif    
    uint8_t buffer[POV_DATA_HEIGHT * MULT];
    rle_decode(imagedata + imageoffsets[col],
	       (unsigned char *)&buffer, POV_DATA_HEIGHT * MULT);
    for (size_t i = 0; i < POV_DATA_HEIGHT; i++) {
      Color16 color;
#ifdef POV_RGB
      color = Color8(buffer[i*3], buffer[i*3+1], buffer[i*3+2]);
#elif defined(POV_8BIT)
      if (buffer[i] > NELEM(pov_color_map)) {
	fprintf(stderr, "Color out of range!\n");
	exit(1);
      }
      color = pov_color_map[buffer[i]];
#else
      color = image_color * buffer[i];
#endif

      // Transform from linear space to sRGB
      RGB<float> color2 = RGB<float>(pow(color.r / 65535, 1.0/2.2),
				     pow(color.g / 65535, 1.0/2.2),
				     pow(color.b / 65535, 1.0/2.2));
      image.set(col, i, color2);
    }
  }

  write_ppm(image, stdout);
}
