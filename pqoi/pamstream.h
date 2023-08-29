#ifndef PAMSTREAM_H
#define PAMSTREAM_H

#include <string>
#include <string.h>
#include <vector>

bool endswith(const std::string &s, const std::string& ending) {
  if (ending.size() > s.size()) return false;
  return strcasecmp(s.c_str() + s.length() - ending.length(), ending.c_str()) == 0;
}


class PamStream {
public:
  PamStream(const std::string &filename, std::string &scaling_commands) {
    setenv("FILE", filename.c_str(), 1);

    std::string cmd;
    if (isPNG(filename)) {
      cmd = "pngtopam -alphapam $FILE";
    } else {
      cmd = "ffmpeg -i \"$FILE\" -an -pix_fmt yuv444p -f yuv4mpegpipe - | y4mtopnm -P";
    }
    if (!scaling_commands.empty()) {
      cmd += " | " + scaling_commands;
    }
    fprintf(stderr, "Executing: %s\n", cmd.c_str());
    f = popen(cmd.c_str(), "r");
    if (!read_header()) {
      fprintf(stderr, "Failed to read PAM header.\n");
      exit(1);
    }
  }

  bool isPNG(const std::string &filename) {
    return endswith(filename, ".png");
  }

  int c() {
    int c = getc(f);
    return c;
  }

  int getnum() {
    int x = 0;
    while (1) {
      int y = c();
      if (y == ' ' && !x) continue;
      y -= '0';
      if (y < 0 || y > 9) return x;
      x *= 10;
      x += y;
    }
  }

  std::string getword() {
    char word[20];
    int l = 0;
    while (1) {
      word[l] = c();
      if (word[l] == ' ' && l == 0) continue;
      if (word[l] == ' ' || word[l] == '\n' || word[l] == -1 || l == sizeof(word) - 1) {
	word[l] = 0;
	return std::string(word);
      }
      l++;
    }
  }

  bool read_header() {
    int k;
    int c1 = c();
    if (c1 == -1) return false;
    int c2 = c();
    int c3 = c();
    if(c1 != 'P' || c2 != '7' || c3 != '\n') {
      fprintf(stderr, "NOT P7 input! (%c%c)\n", c1, c2);
      goto headerr;
    }

    while(1) {
      std::string word = getword();
//       fprintf(stderr, "WORD=%s\n", word.c_str());
      if (word == "ENDHDR") break;
      if (word == "WIDTH") {
	xsize = getnum();
      } else if (word == "HEIGHT") {
	ysize = getnum();
      } else if (word == "DEPTH") {
	depth = getnum();
      } else if (word == "MAXVAL") {
	maxval = getnum();
      } else if (word == "TUPLTYPE") {
	pair_type = getword();
	// Ignore?
      } else {
	fprintf(stderr, "Uknown header label: %s\n", word.c_str());
	goto headerr;
      }
    }
    return true;

  headerr:
    perror("**while reading header");
    exit(1);

  headeof:
    fprintf(stderr,"**End of file in header\n");
    exit(1);
  }

  bool read_frame(std::vector<uint8_t>& data) {
    if (!first) if (!read_header()) return false;
    first = false;
    size_t bytes = xsize * ysize * depth * wordsize();
    data.resize(bytes);
    return fread(data.data(), 1, bytes, f) == bytes;
  }

  bool first = true;
  int xsize = 0;
  int ysize = 0;
  int depth = 0;
  int maxval = 0;
  std::string pair_type;

private:
  int wordsize() {
    if (maxval == 0xFF) return 1;
    fprintf(stderr, "Maxval should be 255.\n");
    exit(1);
  }

  FILE *f;
};

#endif
