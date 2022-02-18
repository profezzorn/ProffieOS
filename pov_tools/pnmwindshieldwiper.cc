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
#include <unistd.h>
#include <getopt.h>

#include "common.h"

TooDee<RGB<float> > image;
TooDee<RGB<float> > output;

double scale = 0.0001;
double len = 36.0;
double base_len = 18.0;
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

enum TMP {
  bo = 1,
  bi = 2,
  to = 4,
  ti = 8,
};

static struct option long_options[] = {
  {"width",    required_argument, 0, 'w'},
  {"height",   required_argument, 0, 'h'},
  {"length",   required_argument, 0, 'l'},
  {"offset",   required_argument, 0, 'o'},
  {"degrees",  required_argument, 0, 'd'},
  {"scale",    required_argument, 0, 's'},
  {"fill",     no_argument,       0, 'f'},
  {"help",     no_argument,       0, '?'}
};

int main(int argc, char** argv) {
  int pixel_scale = 10;
  int pixel_width = 0;
  int pixel_height = 144;
  int fill = 0;
  opterr = 0;
  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "w:h:b:o:df", long_options, &option_index);
    if (c == -1) break;
    switch (c) {
      case 'w': pixel_width = atoi(optarg); break;
      case 'h': pixel_height = atoi(optarg); break;
      case 's': pixel_scale = atoi(optarg); break;
      case 'l': len = atof(optarg); break;
      case 'o': base_len = atof(optarg); break;
      case 'd': max_angle = atof(optarg) * M_PI / 180.0; break;
      case 'f': fill = 1; break;
      case '?':
	fprintf(stderr, "%s [options]\n", argv[0]);
	fprintf(stderr, " -h N / --height=N  : Set height, defaults to 144.\n");
	fprintf(stderr, " -w N / --width=N   : Set width, defaults to height * 3\n");
	fprintf(stderr, " -s N / --scale=N   : Set oversampling scale, defaults to 10\n");
	fprintf(stderr, " -l N / --length=N  : Set blade length, defauls to 36\n");
	fprintf(stderr, " -o N / --offset=N  : Set fulcrum offset, defaulits to 18\n");
	fprintf(stderr, " -d N / --degrees=N : Set wipe degrees, defaults to 90\n");
	fprintf(stderr, " -f / --fill        : Scale image to fill entire wipe\n");
	exit(1);
    }
  }

  if (pixel_width == 0) pixel_width = pixel_height * 3;
  
  read_pnm(stdin, &image);
  output.resize(pixel_width * pixel_scale, pixel_height * pixel_scale);

  if (fill) {
    double R = base_len + len;
    double top = R;
    double bottom = base_len * cos(max_angle);
    double W = R * sin(max_angle) * 2.0;
    double H = top - bottom;
    double Wscale = W / image.xsize();
    double Hscale = H / image.ysize();
    scale = std::max<double>(Hscale, Wscale);
    height = (top + bottom) / 2.0;
  } else {
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
  }

  fprintf(stderr, "\nscale = %f  height = %f\n", scale, height);

  for (int y = 0; y < output.ysize(); y++)
    for (int x = 0; x < output.xsize(); x++)
      output.set(x, y, sample(x, y));

  write_ppm(output, stdout);
}

