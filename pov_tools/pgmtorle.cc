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

  RGB<float> brightest;
  brightest.r = 0.0;
  brightest.g = 0.0;
  brightest.b = 0.0;
  for (int x = 0; x < image.xsize(); x++) {
    std::vector<unsigned char> data;
    for (int y = image.ysize() - 1; y >= 0; y--) {
      data.push_back((int)(pow(image.get(x, y).gray(), 2.2) * 255.0));
      if (image.get(x,y).gray() > brightest.gray()) {
        brightest = image.get(x,y);
      }
//      data.push_back((int)(pow(image.get(x, y).r, 2.2) * 255.0));
//      data.push_back((int)(pow(image.get(x, y).g, 2.2) * 255.0));
//      data.push_back((int)(pow(image.get(x, y).b, 2.2) * 255.0));
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
  printf("const Color8 image_color(%d, %d, %d);\n",
	 (int)(pow(brightest.r, 2.2) * 255.0),
	 (int)(pow(brightest.g, 2.2) * 255.0),
	 (int)(pow(brightest.b, 2.2) * 255.0));
  printf("#define POV_DATA_HEIGHT (%ld)\n", image.ysize());
  fprintf(stderr, " %d bytes\n", (int)(offsets.size() * 2 + blob.size()));
}
