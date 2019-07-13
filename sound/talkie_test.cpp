#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

// junk needed to make this hack work.
#define digitalWrite(X, Y) ((void)0)
#define noInterrupts() ((void)0)
#define interrupts()  ((void)0)
#define pgm_read_byte(X) *((uint8_t*)(X))
#define NELEM(X) (sizeof(X)/sizeof((X)[0]))
#define EnableAmplifier()
#define PROGMEM

int32_t clampi32(int32_t x, int32_t a, int32_t b) {
  if (x < a) return a;
  if (x > b) return b;
  return x;
}
int16_t clamptoi16(int32_t x) {
  return clampi32(x, -32768, 32767);
}

class STDOUTHELPER {
public:
  template<class F>
  void println(F foo) {
    std::cout << foo << std::endl;
  }
};
STDOUTHELPER STDOUT;

#include "talkie.h"

CommandParser* parsers = NULL;

#define STRINGIFY(X) std::string((char *)&(X), sizeof(X))

std::string mkchunk(std::string chnk,
		    std::string data) {
  std::string ret = chnk;
  uint32_t tmp = data.size();
  ret += STRINGIFY(tmp);
  ret += data;
  return ret;
}

int main(int argc, char** argv) {
  std::string tmp;
  Talkie talkie;

  int rate = 25;
  int i = 1;
  if (argc > 1 && argv[1][0] == '-') {
    rate = atoi(argv[1]+1);
    i++;
  }

  std::string samples = "";
  for (; i < argc; i++) {
    std::vector<uint8_t> data;
    const char *arg = argv[i];
    const char *start = strchr(arg, '{');
    const char *end = strchr(arg, '}');
    if (!start) start = arg;
    if (!end) end = arg + strlen(arg);
    int n = 0;
    int digits = 0;
    for (;start < end; start++) {
      if (*start >= '0' && *start <= '9') {
	n = (n << 4) + *start - '0';
	digits++;
      } else if (*start >= 'a' && *start <= 'f') {
	n = (n << 4) + *start - 'a' + 10;
	digits++;
      } else if (*start >= 'A' && *start <= 'F') {
	n = (n << 4) + *start - 'A' + 10;
	digits++;
      } else if (*start == 'x') {
	digits = 0;
      }
      if (digits == 2) {
	data.push_back(n);
	fprintf(stderr, "%02x ", n);
	fflush(stderr);
	digits = n = 0;
      }
    }
    talkie.Say(data.data(), rate);
    while (talkie.isPlaying()) {
      int16_t ret = talkie.Get44kHz();
      samples += STRINGIFY(ret);
    }
  }
  
  struct Fmt {
    uint16_t pcm = 1;
    uint16_t channels = 1;
    uint32_t rate = 44100;
    uint32_t byterate = 88200;
    uint16_t block_align = 2;
    uint16_t bits_per_sample = 16;
  };

  Fmt fmt;
  std::string wav = mkchunk("RIFF",
			    "WAVE" +
			    mkchunk("fmt ", STRINGIFY(fmt)) +
			    mkchunk("data", samples));
  fwrite(wav.c_str(), 1, wav.size(), stdout);
}
