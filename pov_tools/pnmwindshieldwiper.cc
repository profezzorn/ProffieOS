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

template<class T>
struct RGB {
  T r, g, b;
  RGB() {}
  RGB(T a, T b, T c) : r(a), g(b), b(c) {}
  T max() const {
    if (r > g && r > b) return r;
    if (g > b) return g;
    return b;
  }
  RGB operator+(const RGB& other) const {
    RGB ret = *this;
    ret.r += other.r;
    ret.g += other.g;
    ret.b += other.b;
    return ret;
  }
  RGB operator*(float m) const {
    RGB ret = *this;
    ret.r *= m;
    ret.g *= m;
    ret.b *= m;
    return ret;
  }
};

template<class T>
class TooDee {
public:
  TooDee() : xsize_(0), ysize_(0) {}
  TooDee(size_t xsize, size_t ysize) : xsize_(xsize), ysize_(ysize), data_(xsize * ysize) {}
  void resize(int xsize, int ysize) {
    xsize_ = xsize;
    ysize_ = ysize;
    data_.resize(xsize * ysize);
  }
  void set(int x, int y, const T& value) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < xsize_);
    assert(y < ysize_);
    data_[xsize_ * y + x] = value;
  }
  const T& get(int x, int y) const {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < xsize_);
    assert(y < ysize_);
    return data_[xsize_ * y + x];
  }
  const T& get_clamped(int x, int y) const {
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= xsize_) x = xsize_ - 1;
    if (y >= ysize_) y = ysize_ - 1;
    return data_[xsize_ * y + x];
  }
  T& at(int x, int y) {
    assert(x >= 0);
    assert(y >= 0);
    assert(x < xsize_);
    assert(y < ysize_);
    return data_[xsize_ * y + x];
  }
  const size_t xsize() const { return xsize_; }
  const size_t ysize() const { return ysize_; }
private:
  size_t xsize_;
  size_t ysize_;
  std::vector<T> data_;
};


float read_component(FILE *f, int type, int depth) {
  int v = 0;
  float div = depth;
  if (type <= 3) {
    if (!fscanf(f, "%d", &v)) exit(1);
  } else {
    while (depth) {
      v = (v << 8) + getc(f);
      depth >>= 8;
    }
  }
  return v / div;
}

RGB<float> read_pixel(FILE* f, int type, int depth) {
  RGB<float> ret;
  if (type == 2 || type == 5) {
    ret.r = ret.g = ret.b = read_component(f, type, depth);
  } else {
    ret.r = read_component(f, type, depth);
    ret.g = read_component(f, type, depth);
    ret.b = read_component(f, type, depth);
  }
  return ret;
}

int read_pnm(FILE* f, TooDee<RGB<float> >* image) {
  size_t xsize, ysize;
  int depth;
  int type;
  if (fscanf(f, "P%d\n%lu %lu\n%d\n", &type, &xsize, &ysize, &depth) != 4) {
    return 0;
  }
  image->resize(xsize, ysize);
  for (size_t y = 0; y < ysize; y++) {
    for (size_t x = 0; x < xsize; x++) {
      image->set(x, y, read_pixel(f, type, depth));
    }
  }
  return depth;
}

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

