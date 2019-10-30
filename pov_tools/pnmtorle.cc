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

void rle_encode(const unsigned char *input,
                int input_length,
                unsigned char *output,
                int *output_length) {
  int i, l, e;
  int olen = 0;
  unsigned char *cnt = 0;
#define VALID_CNT (cnt && *cnt < 255 && cnt+*cnt+2==output+olen)
  assert(*output_length >= input_length + 2 + (input_length >> 7));

  for (i = 0; i < input_length; ) {
    int best = -1;
    int best_offset = -1;
    int l, p;
    for (p = 0; p < 256; p++) {
      int q;
      if (i - p - 1 < 0) break;
      for (l = 0; l < 256 && i+l < input_length; l++) {
        if (input[i+l] != input[i-1-p+l]) break;
      }
      if (l > best) {
        best = l;
        best_offset = p;
      }
    }

    for (l = 1; l < input_length - i; l++) {
      if (input[i] != input[i+l]) {
        break;
      }
    }
    if (best > l+1 && best > (VALID_CNT ? 3 : 2)) {
      output[olen++] = 255;
      output[olen++] = best_offset;
      output[olen++] = best;
      i += best;
    } else if (l >= (VALID_CNT ? 3 : 2)) {
      if (l > 127) {
        l = 127;
      }
      output[olen++] = 128 + (l - 2);
      output[olen++] = input[i];
      i += l;
    } else {
      if (VALID_CNT) {
        (*cnt)++;
      } else {
        cnt = output+olen;
        output[olen++] = 0;
      }
      output[olen++] = input[i];
      i++;
    }
  }
  assert(olen <= *output_length);
  *output_length = olen;
}

int main(int argc, char** argv) {
  TooDee<RGB<float> > image;
  read_pnm(stdin, &image);
  std::vector<unsigned char> blob;
  std::vector<std::pair<int, int> > offsets;

  for (int x = 0; x < image.xsize(); x++) {
    std::vector<unsigned char> data;
    for (int y = image.ysize() - 1; y >= 0; y--) {
      data.push_back((int)(pow(image.get(x, y).r, 2.2) * 255.0));
      data.push_back((int)(pow(image.get(x, y).g, 2.2) * 255.0));
      data.push_back((int)(pow(image.get(x, y).b, 2.2) * 255.0));
#if 0
      if (data[data.size()-1] != 0 || 
	  data[data.size()-2] != 0 ||
	  data[data.size()-3] != 0) {
	static int cnt = 0;
	if (cnt++ < 100) {
	  fprintf(stderr, "%d %d %d %f %f %f\n",
		  data[data.size()-3],
		  data[data.size()-2],
		  data[data.size()-1],
		  image.get(x,y).r,
		  image.get(x,y).g,
		  image.get(x,y).b);
		  
	}
      }
#endif
    }
    std::vector<unsigned char> output(data.size() * 2 + 2);
    int output_length = output.size();
    rle_encode(data.data(), data.size(), output.data(), &output_length);
    output.resize(output_length);
    int pos;
    for (pos = 0; pos < blob.size(); pos++) {
      if (!memcmp(output.data(), blob.data() + pos, std::min<int>(output.size(), blob.size() - pos))) {
	break;
      }
    }
    while (blob.size() < pos + output.size()) {
      blob.push_back(output[blob.size() - pos]);
    }
    offsets.push_back( std::make_pair<int, int>(pos, output.size()) );
  }

  printf("const unsigned char imagedata[] = {");
  for (int i = 0; i < blob.size(); i++) {
    if (i) printf(",");
    if (!(i % 15)) printf("\n");
    printf(" %d", blob[i]);
  }
  printf("\n};\n");
  printf("const uint16_t imageoffsets[] = {");
  for (int i = 0; i < offsets.size(); i++) {
    if (i) printf(",");
    if (!(i % 10)) printf("\n");
    printf(" %d", offsets[i].first);
  }
  printf("};\n");
  printf("#define POV_RGB\n");
  fprintf(stderr, " %d bytes\n", (int)(offsets.size() * 2 + blob.size()));
}
