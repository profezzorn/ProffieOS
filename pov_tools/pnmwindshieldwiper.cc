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

TooDee<RGB<float> > image;
TooDee<RGB<float> > output;

double scale = 0.0001;
double len = 20.0;
double base_len = 10.0;
double height = base_len + len / 2.0;
double max_angle = M_PI / 4.0;

RGB<float> sample(int x, int y) {
  double angle = -max_angle  + x * max_angle * 2.0 / output.xsize();
  double dx = sin(angle);
  double dy = cos(angle);
  float l = base_len + len - len * y / output.ysize();
  float X = dx * l;
  float Y = dy * l;
  Y -= height;
  Y = - Y;
  X /= scale;
  Y /= scale;
  X += image.xsize() / 2.0;
  Y += image.ysize() / 2.0;
  x = (int)round(X);
  y = (int)round(Y);
  if (x < 0 || y < 0 || x >= image.xsize() || y >= image.ysize())
    return RGB<float>(0.0, 0.0, 0.0);
  return image.get(x, y);
}

bool zero(int x, int y) {
  RGB<float> tmp = sample(x, y);
  return tmp.r < 0.1 && tmp.g < 0.1 && tmp.b < 0.1;
}

void write_ppm(const TooDee<RGB<float> >& tmp, FILE* f) {
  int maxvalue = 255;
  fprintf(f, "P6\n%lu %lu\n%d\n", tmp.xsize(), tmp.ysize(), maxvalue);
  for (size_t y = 0; y < tmp.ysize(); y++) {
    for (size_t x = 0; x < tmp.xsize(); x++) {
      putc(std::min<int>(maxvalue, tmp.get(x, y).r * maxvalue), f);
      putc(std::min<int>(maxvalue, tmp.get(x, y).g * maxvalue), f);
      putc(std::min<int>(maxvalue, tmp.get(x, y).b * maxvalue), f);
    }
  }
}

enum TMP {
  bo = 1,
  bi = 2,
  to = 4,
  ti = 8,
};

int main(int argc, char** argv) {
  read_pnm(stdin, &image);
  output.resize(800*10, 144*10);

  for (int i = 0; i < 1000; i++) {
    int flags = 0;
    for (int x = 0; x < output.xsize() && flags != 0xf; x++) {
      if (!zero(x, -1)) flags |= to | ti;
      if (!zero(x, 0)) flags |= ti;
      if (!zero(x, output.ysize() - 1)) flags |= bi;
      if (!zero(x, output.ysize())) flags |= bo | bi;
    }
    for (int y = 0; y < output.ysize() && flags != 0xf; y++) {
      if (!zero(-1, y)) flags |= bo | bi;
      if (!zero(0, y)) flags |= bi;
      if (!zero(output.xsize() - 1, y)) flags |= bi;
      if (!zero(output.xsize(), y)) flags |= bi | bo;
    }
    fprintf(stderr, "\r scale = %f  height = %f flags = %x ", scale, height, flags);
    switch (flags) {
      case bi | ti:
	// Perfect, done
	break;
      case 0:
	scale *= 1.1333333333;
	continue;
      case bo | bi | to | ti:
	scale *= 0.99;
	continue;
      case bo | bi | ti:
	height += 0.011;
	scale *= 0.999;
	continue;
      case bi | to | ti:
	height -= 0.0711231;
	scale *= 0.999;
	continue;
      case to | ti:
	height -= 0.53;
	continue;
      case ti:
	scale *= 1.01;
	height -= 0.0022;
	continue;
      case bo | bi:
	height += 0.57;
	continue;
      case bi:
	scale *= 1.011;
	height += 0.00255;
	continue;

      default:
	fprintf(stderr, "UNKNOWN FLAGS: %x\n", flags);
	exit(1);
    }
    break;
  }

  for (int y = 0; y < output.ysize(); y++)
    for (int x = 0; x < output.xsize(); x++)
      output.set(x, y, sample(x, y));

  write_ppm(output, stdout);
}

