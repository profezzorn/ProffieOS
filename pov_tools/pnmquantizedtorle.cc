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

int main(int argc, char** argv) {
  TooDee<RGB<float> > image;
  read_pnm(stdin, &image);
  std::vector<unsigned char> blob;
  std::vector<std::pair<int, int> > offsets;

  RGB<float> colormap[256];
  int num_colors = 0;

  for (int x = 0; x < image.xsize(); x++) {
    std::vector<unsigned char> data;
    for (int y = image.ysize() - 1; y >= 0; y--) {
      int c;
      for (c = 0; c < num_colors; c++) {
	if (image.get(x, y) == colormap[c]) {
	  data.push_back(c);
	  break;
	}
      }
      if (c == num_colors) {
	if (num_colors == 256) {
	  fprintf(stderr, "Too many colors in image!\n");
	  exit(1);
	}
	data.push_back(c);
	colormap[num_colors] = image.get(x, y);
	num_colors++;
      }
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
    offsets.push_back( std::pair<int, int>(pos, output.size()) );
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
  printf("const Color16 pov_color_map[] = {");
  for (int i = 0; i < num_colors; i++) {
    if (i) printf(",");
    printf("\n  Color16(%d, %d, %d)",
	   (int)(pow(colormap[i].r, 2.2) * 65535.0),
	   (int)(pow(colormap[i].g, 2.2) * 65535.0),
	   (int)(pow(colormap[i].b, 2.2) * 65535.0));
  }
  printf("};\n");
  printf("#define POV_8BIT\n");
  printf("#define POV_DATA_HEIGHT (%ld)\n", image.ysize());
  fprintf(stderr, " %d bytes\n", (int)(offsets.size() * 2 + blob.size() + num_colors * 6));
}
