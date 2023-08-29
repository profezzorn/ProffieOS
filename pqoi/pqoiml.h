#ifndef PQOIML_H
#define PQOIML_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <string.h>
#include <math.h>
#include "pamstream.h"
#include "pqoi.h"

bool near(float a, float b) { return fabs(a-b) < 0.02; }

std::pair<int, int> getfps(const std::string& filename) {
  setenv("FILE", filename.c_str(), 1);
  std::string cmd = "ffprobe -i \"$FILE\" 2>&1 |  sed -n 's@.*Video:.*, \\([0-9.]*\\) fps,.*@\\1@gp'";
  fprintf(stderr, "Executing: %s\n", cmd.c_str());
  FILE *f = popen(cmd.c_str(), "r");
  if (!f) return std::make_pair(0, 0);
  char buffer[128];
  int l = fread(buffer, 1, 128 - 1, f);
  if (l < 0) return std::make_pair(0, 0);
  buffer[l] = 0;
  fclose(f);
  float fps  = strtod(buffer, nullptr);
  if (fps <= 0.0) return std::make_pair(0, 0);
  int i = (int)floor(fps + 0.5);
  if (near(fps, i)) return std::make_pair(i, 1);
  if (near(fps, i / 1.001)) return std::make_pair(i * 1000, 1001);
  return std::make_pair(fps, 1000000);
}


class Generator {
public:
  void Update(std::string& s, uint32_t pos, uint32_t v) {
    s[pos+0] = v & 0xff;
    s[pos+1] = (v >> 8) & 0xff;
    s[pos+2] = (v >> 16) & 0xff;
    s[pos+3] = (v >> 24) & 0xff;
  }
  void AddChunk(std::string chunk) {
    size_t len = chunk.size();
    // Fix the length.
    Update(chunk, 4, len - 8);
    chunks.push_back(chunk);
    total_size += chunk.size();
  }
  void AddLabel(const std::string& label) {
    label_positions[label] = total_size;
  }
  void AddGoto(const std::string& label) {
    gotos[chunks.size()] = label;
  }
  void Output(FILE* f) {
    // Fix gotos
    for (auto g : gotos) {
      Update(chunks[g.first], 12, label_positions[g.second]);
    }
    for (const std::string& chunk : chunks) {
      fwrite(chunk.c_str(), 1, chunk.size(), f);
    }
  }
  bool has_fps = false;
private:
  std::map<std::string, size_t> label_positions;
  std::vector<std::string> chunks;
  std::map<size_t, std::string> gotos;
  size_t total_size = 0;
};

class Pqoiml {
public:
  struct PqoimlLine {
    std::string label;
    std::string goto_label;
    uint32_t condition;
    std::string file;
    uint32_t fps_num;
    uint32_t fps_den;

    void skipspace(const char* &x) {
      while (isspace(*x)) x++;
    }

    bool word(const char* &x, const char* w) {
      size_t l = strlen(w);
      if (strlen(x) < strlen(w)) return false;
      for (int i = 0; i < l; i++) {
	if (tolower(x[i]) != tolower(w[i])) return false;
      }
      if (x[l] == 0 || isspace(x[l]) || x[l] == '#') {
	x += l;
	skipspace(x);
	return true;
      }
      return false;
    }

    std::string rest(const char* x) {
      const char *end = strchr(x, '#');
      if (end == nullptr) end = x + strlen(x);
      return std::string(x, end);
    }

    uint32_t parse_integer(const char* &x) {
      uint32_t ret = 0;
      while (*x >= '0' && *x <= '9') {
	ret = ret * 10 + *x - '0';
	x++;
      }
      return ret;
    }

    uint32_t parse_condition(const char* &x) {
      skipspace(x);
      if (!*x || *x == ' ') return 0;
      uint32_t ret = *(x++);
      if (!*x || *x == ' ') return ret;
      ret <<= 8;
      ret |= *(x++);
      if (!*x || *x == ' ') return ret;
      ret <<= 8;
      ret |= *(x++);
      if (!*x || *x == ' ') return ret;
      ret <<= 8;
      ret |= *(x++);
      return ret;
    }


    bool parse(const std::string& line, Pqoiml* p) {
      const char* x = line.c_str();
//      fprintf(stderr, "Parsing line: %s\n", x);
      skipspace(x);
      if (word(x, "if")) {
	condition = parse_condition(x);
	if (!word(x, "goto")) {
	  fprintf(stderr, "Missing goto after 'if'");
	  return false;
	}
	goto_label = std::string(x);
	return true;
      }
      if (word(x, "goto")) {
	goto_label = rest(x);
	return true;
      }
      if (word(x, "label")) {
	label = rest(x);
	return true;
      }
      if (word(x, "file")) {
	file = rest(x);
	return true;
      }
      if (word(x, "fps")) {
	fps_num = parse_integer(x);
	if (*x != ':') {
	  return false;
	}
	x++;
	fps_den = parse_integer(x);
	return true;
      }
      if (word(x, "scaling")) {
	p->set_default_scaling_commands(rest(x));
	return true;
      }
      fprintf(stderr, "Failed to parse line: %s\n", line.c_str());
      return false;
    }

    std::string mkint(uint32_t x) {
      uint8_t str[4];
      str[0] = x & 0xff;
      str[1] = (x >> 8) & 0xff;
      str[2] = (x >> 16) & 0xff;
      str[3] = (x >> 24) & 0xff;
      return std::string(str, str+4);
    }

    std::string mkheader(std::string magic,
		  uint32_t data1,
		  uint32_t data2) {
      return magic + mkint(0) + mkint(data1) + mkint(data2);
    }

    void generate(Generator* out, Pqoiml* p) {
      if (!label.empty()) {
	out->AddLabel(label);
	return;
      }
      if (!file.empty()) {
	std::pair<int, int> fps = getfps(file);
	fprintf(stderr, "FPS: %d:%d\n", fps.first, fps.second);
	if (fps.first && !out->has_fps) {
	  out->has_fps = true;
	  out->AddChunk(mkheader("PQVF", fps.first, fps.second));
	}
	PamStream stream(file, p->scaling_commands);
	std::vector<uint8_t> pixels;
	while (stream.read_frame(pixels)) {
	  PQOI::EasyPqoiEncoder pqoiencoder;
	  PQOI::EasyPqoiEncoder::RGB565Data rgb565data = pqoiencoder.quantize(pixels.data(), stream.xsize, stream.ysize, stream.depth);
	  std::vector<uint8_t> encoded_pqoi = pqoiencoder.encode(rgb565data);
	  out->AddChunk(std::string(encoded_pqoi.begin(), encoded_pqoi.end()));
	}
	return;
      }
      if (!goto_label.empty()) {
	out->AddGoto(goto_label);
	out->AddChunk(mkheader("GOTO", condition, 0));
	return;
      }
      if (fps_den || fps_num) {
	out->AddChunk(mkheader("PQVF", fps_num, fps_den));
	out->has_fps = true;
	return;
      }
      fprintf(stderr, "Illegal LINE!\n");
      exit(1);
    }

    void dump(FILE* f) {
      if (!label.empty()) {
	fprintf(f, "label %s\n", label.c_str());
	return;
      }
      if (!file.empty()) {
	fprintf(f, "file %s\n", file.c_str());
	return;
      }
      if (!goto_label.empty()) {
	if (condition) {
	  fprintf(f, "if %c%c%c%c ",
		  condition & 0xff,
		  (condition >> 8) & 0xff,
		  (condition >> 16) & 0xff,
		  (condition >> 24) & 0xff);
	}
	fprintf(f, "goto %s\n", goto_label.c_str());
	return;
      }
      if (fps_den || fps_num) {
	fprintf(f, "fps %d:%d\n", fps_num, fps_den);
	return;
      }
      fprintf(f, "# Illegal LINE!\n");
      fprintf(stderr, "Illegal LINE!\n");
    }
  };

  std::string scaling_commands;
  std::vector<PqoimlLine> lines;

  void set_default_scaling_commands(const std::string& commands) {
    scaling_commands = commands;
  }

  bool parse(const std::string& line) {
    // fprintf(stderr, "PARSE: %s\n", line.c_str());
    lines.resize(lines.size() + 1);
    return lines.back().parse(line, this);
  }

  void parseFile(const std::string& filename) {
    char buffer[1024];
    FILE* f = fopen(filename.c_str(), "r");
    if (!f) {
      perror("open file");
      exit(1);
    }
    int line = 1;
    while (fgets(buffer, sizeof(buffer), f)) {
      if (!parse(std::string(buffer))) {
	fprintf(stderr, "On line %d\n", line);
	exit(1);
      }
      line++;
    }
    fclose(f);
  }


  void Generate(Generator* out) {
    for (auto line : lines) {
      line.generate(out, this);
    }
  }
  void dump(FILE* f) {
    for (auto line : lines) line.dump(f);
  }

  void optimize() {
    std::set<std::string> used_gotos;
    for (auto line : lines) {
      if (!line.goto_label.empty()) {
	used_gotos.insert(line.goto_label);
      }
    }
    size_t n = 0;
    for (size_t i = 0; i < lines.size(); i++) {
      if (!lines[i].label.empty()) {
	if (used_gotos.find(lines[i].label) == used_gotos.end()) {
	  continue;
	}
      }
      if (i != n) lines[n] = lines[i];
      n++;
    }
    lines.resize(n);
  }
};

#endif

