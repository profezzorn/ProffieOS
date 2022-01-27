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
  T gray() const {
    return std::max<T>(r, std::max<T>(g, b));
  }
  bool operator==(const RGB& other) const {
    return r == other.r && g == other.g && b == other.b;
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
