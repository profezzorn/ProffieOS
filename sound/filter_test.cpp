// Applies filtering to a wav file.
// File is assumed to be 44100 16-bit PCM mono.
// File is read from stdin, filtered output is written to stdout.

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define AUDIO_RATE 44100
#define FILTER_CUTOFF_FREQUENCY 150
#define FILTER_ORDER 10

#include "filter.h"

#define STRINGIFY(X) std::string((char *)&(X), sizeof(X))

template<class T>
T UNSTRINGIFY(const char* tmp) {
  T ret;
  memcpy(&ret, tmp, sizeof(T));
  return ret;
}

std::string mkchunk(std::string chnk,
		    std::string data) {
  std::string ret = chnk;
  uint32_t tmp = data.size();
  ret += STRINGIFY(tmp);
  ret += data;
  return ret;
}

std::string readAll() {
  std::string ret = "";
  while (true) {
    char tmp[4096];
    int len = fread(tmp, 1, sizeof(tmp), stdin);
    if (!len) return ret;
    ret += std::string(tmp, len);
  }
}

uint32_t chunkLength(std::string chnk) {
  return UNSTRINGIFY<uint32_t>(chnk.c_str()+4);
}

std::string chunkType(std::string chnk) {
  return chnk.substr(0, 4);
}

std::string deChunk(std::string chnk) {
  return chnk.substr(8, chunkLength(chnk));
}

std::string skipChunk(std::string chnk) {
  return chnk.substr(8+chunkLength(chnk));
}

std::string readWav(std::string wav) {
  if (chunkType(wav) != "RIFF") {
    fprintf(stderr, "Not a RIFF\n");
    return "";
  }
  wav = deChunk(wav);
  if (chunkType(wav) != "WAVE") {
    fprintf(stderr, "Not a WAVE\n");
    return "";
  }
  wav = wav.substr(4);
  std::string ret = "";
  if (chunkType(wav) != "data ") {
    wav = skipChunk(wav);
  }
  return deChunk(wav);
}


int main(int argc, char** argv) {
  static Filter::Biquad<
    Filter::Bilinear<
    Filter::BLT<
      Filter::ConvertToHighPass<
	Filter::ButterWorthProtoType<FILTER_ORDER>, FILTER_CUTOFF_FREQUENCY, AUDIO_RATE>>>> filter;

  std::string input = readWav(readAll());
  std::string samples = "";
  for (int i = 0; i < input.length() / 2 / 4; i++) {
    float tmp[4];
    for (int j = 0; j < 4; j++) {
      tmp[j] = UNSTRINGIFY<int16_t>(input.c_str() + (i*4+j)*2) / 32768.0;
    }
    filter.Run4(tmp);
    for (int j = 0; j < 4; j++) {
      int16_t sample = tmp[j] * 32768.0;
      samples += STRINGIFY(sample);
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
