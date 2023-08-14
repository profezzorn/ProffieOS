#include "pqoi.h"

#include <string>
#include <map>
#include <utility>
#include <cassert>

#include "pqoiml.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

template<typename T> 
bool Read(FILE* f, T* data) {
  return fread(data, 1, sizeof(T), f) == sizeof(T);
}

struct CommonPQVHeader {
  uint32_t magic;
  uint32_t length;
  uint32_t data1;
  uint32_t data2;
};

constexpr uint32_t onecc(char c, int shift) {
  return ((uint32_t)c) << shift;
}
constexpr uint32_t fourcc(char const p[5] ) {
  return onecc(p[0], 0) + onecc(p[1], 8) + onecc(p[2], 16) + onecc(p[3], 24);
}

template<class T>
std::string F(const char *a, T x) {
  char buffer[1024];
  sprintf(buffer, a, x);
  return buffer;
}

std::string format_condition(uint32_t x) {
  char tmp[5];
  tmp[0] = x & 0xff;
  x >>= 8;
  tmp[1] = x & 0xff;
  x >>= 8;
  tmp[2] = x & 0xff;
  x >>= 8;
  tmp[3] = x & 0xff;
  tmp[4] = 0;
  return std::string(tmp);
}


int main(int argc, char **argv) {
  int image_number = 0;
  Pqoiml pqoiml;
  FILE* f = fopen(argv[1], "rb");
  if (!f) {
    perror("open file");
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  size_t l = ftell(f);
  fseek(f, 0, SEEK_SET);
  while (ftell(f) < l) {
    pqoiml.parse(F("label L%d", ftell(f)));
    CommonPQVHeader header;
    if (!Read(f, &header)) {
      fprintf(stderr, "Failed to read header @ %ld.\n", ftell(f));
      exit(1);
    }
    bool alpha = false;
    switch (header.magic) {
      case fourcc("GOTO"):
	if (header.data1) {
	  pqoiml.parse("if " + format_condition(header.data1) + F(" goto L%d", header.data1));
	} else {
	  pqoiml.parse(F("goto L%d", header.data2));
	}
	break;

      case fourcc("PQVF"):
	pqoiml.parse(F("fps %d", header.data1) + F(":%d", header.data2));
	break;

      case fourcc("PQOA"): alpha = true;
      case fourcc("PQOI"): {
	std::vector<uint8_t> indata(header.length + 8);
	memcpy(indata.data(), &header, sizeof(header));
	if (fread(indata.data() + 16, 1, header.length - 8, f) != header.length - 8) {
	  fprintf(stderr, "Failed to read image data.");
	}
	PQOI::EasyPqoiEncoder decoder;
	PQOI::EasyPqoiEncoder::RGB565Data data = decoder.decode(indata);
	int channels = alpha ? 4 : 3;
	int total_bytes = data.w * data.h * channels;
	std::vector<uint8_t> rgba(total_bytes);
	uint8_t *out = rgba.data();
	for (int y = 0; y < data.h; y++) {
	  for (int x = 0; x < data.w; x++) {
	    uint16_t pixel = data.pixels[y * data.w + x];
	    int R = pixel >> 11;
	    int G = (pixel >> 5) & 63;
	    int B = pixel & 31;
	    int A = 0;
	    if (alpha) {
	      A = data.alphas[y * data.w + x];
	      int Ainv = 32 - A;
	      // Un-pre-multiply
	      R = R * 32 / Ainv;
	      G = G * 32 / Ainv;
	      B = B * 32 / Ainv;
	    }
	    // Check if these are correct...
	    *(out++) = (R * 0x21) >> 2;
	    *(out++) = (G * 0x41) >> 4;
	    *(out++) = (B * 0x21) >> 2;
	    if (alpha) *(out++) = 255 - A * 255 / 32;
	  }
	}
	std::string filename = F("IMG%05d.png", image_number++);
	// Save image
	stbi_write_png(filename.c_str(), data.w, data.h, channels, rgba.data(), channels * data.w);
	pqoiml.parse("file " + filename);
	break;
      }
      default:
	fprintf(stderr, "Unknown chunk type %c%c%c%c @ %ld\n",
		header.magic & 0xff,
		(header.magic >> 8) & 0xff,
		(header.magic >> 16) & 0xff,
		(header.magic >> 24) & 0xff,
		ftell(f));
	exit(1);
      
    }
  }
  fclose(f);
  pqoiml.optimize();
  pqoiml.dump(stdout);
}

