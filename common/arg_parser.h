#ifndef COMMON_ARG_PARSER_H
#define COMMON_ARG_PARSER_H

#include "strfun.h"

class ArgParserInterface {
public:
  virtual const char* GetArg(int arg_num,
			     const char* name,
			     const char* default_value) = 0;
  virtual void Shift(int num) = 0;
};

bool FirstWord(const char *str, const char *word) {
  while (*str == ' ' || *str == '\t') str++;
  while (*str == *word) {
    if (!*word) return true;
    str++;
    word++;
  }
  if (*word) return false;
  if (*str == ' ' || *str == '\t') return true;
  return false;
}

class ArgParser : public ArgParserInterface {
public:
  ArgParser(const char* data) : str_(data) {}

  const char* GetArg(int arg_num,
		     const char* name,
		     const char* default_value) override {
    const char* ret = str_;
    int arg = 0;
    while (true) {
      while (*ret == ' ' || *ret == '\t') ret++;
      if (!*ret) return default_value;
      if (++arg == arg_num) {
	if (*ret == '~') return default_value;
	return ret;
      }
      while (*ret && *ret != ' ' && *ret != '\t') ret++;
    }
  }
  void Shift(int words) override {
    while (words-- > 0) str_ = SkipWord(str_);
  }
protected:
  const char* str_;
};

class ArgParserPrinter : public ArgParser {
public:
  ArgParserPrinter(const char* data) : ArgParser(data), data_(data) {}
  const char* GetArg(int arg_num,
		     const char* name,
		     const char* default_value) override {
    arg_num += offset;
    if (current_arg == arg_num) {
      STDOUT.print(name);
      STDOUT.print(" ");
      STDOUT.println(default_value);
      current_arg++;
    }
    if (arg_num > max_arg) {
      max_arg = arg_num;
    }
    return ArgParser::GetArg(arg_num, name, default_value);
  }
  bool next() {
//    STDOUT << current_arg << " " << max_arg << "\n";
    if (current_arg == start_current_arg && max_arg > current_arg) {
      STDOUT.println("VOID ~");
      current_arg++;
    }
    start_current_arg = current_arg;
    offset = 0;
    str_ = data_; // reset ArgParser
    return current_arg <= max_arg;
  }
  
  void Shift(int words) override {
    offset += words;
    ArgParser::Shift(words);
  }
  int offset = 0;
  int max_arg = 0;
  int current_arg = 1;
  int start_current_arg = 1;
  const char* data_;
};


class GetArgParser : public ArgParser {
public:
  GetArgParser(const char* data, int argument, char* output) : ArgParser(data), current_arg(argument), output_(output) {}

  const char* GetArg(int arg_num,
		     const char* name,
		     const char* default_value) override {
    const char* ret = ArgParser::GetArg(arg_num, name, default_value);
    if (current_arg == arg_num + offset) {

      const char* tmp = SkipWord(ret);
      memcpy(output_, ret, tmp - ret);
      output_[tmp-ret] = 0;
      found = true;
    }
    return ret;
  }
  void Shift(int words) override {
    ArgParser::Shift(words);
    offset += words;
  }
  bool next() {
    offset = 0;
    return found;
  }
private:
  int offset = 0;
  bool found = false;
  int current_arg = 1;
  char* output_;
  const char* str_;
};

template<size_t SIZE>
class BitSet {
public:
  BitSet() {
    for (size_t i = 0; i < NELEM(bits_); i++) {
      bits_[i] = 0;
    }
  }
  bool operator[](size_t bit) const { return !!(bits_[bit >> 5] & (1UL << (bit & 31))); }
  bool get(size_t bit) const { return !!(bits_[bit >> 5] & (1UL << (bit & 31))); }
  void set(size_t bit) { bits_[bit >> 5] |= (1UL << (bit & 31)); }
  void clear(size_t bit) { bits_[bit >> 5] &=~ (1UL << (bit & 31)); }
  void clear() {
    for (size_t i = 0; i < NELEM(bits_); i++) {
      bits_[i] = 0;
    }
  }
  size_t popcount() const {
    size_t ret = 0;
    for (size_t i = 0; i < NELEM(bits_); i++) {
      ret += __builtin_popcount(bits_[i]);
    }
    return ret;
  }
  size_t next(size_t bit) const {
    for (size_t i = 1; i <= SIZE; i++) {
      size_t j = (bit + i) % SIZE;
      if (get(j)) return j;
    }
    return bit;
  }
  size_t prev(size_t bit) const {
    for (size_t i = 1; i <= SIZE; i++) {
      size_t j = (bit + SIZE - i) % SIZE;
      if (get(j)) return j;
    }
    return bit;
  }
  size_t nth(int bit) const {
    for (size_t i = 0; i <= SIZE; i++) {
      if (get(i)) {
	if (bit-- <= 0) {
	  return i;
	}
      }
    }
    return 0;
  }
  void operator>>=(int bits) {
    if (!bits) return;
    for (int i = 0; i < (int)SIZE; i++) {
      if (i + bits < (int)SIZE && get(i + bits)) {
	set(i);
      } else {
	clear(i);
      }
    }
  }
  
private:
  uint32_t bits_[(SIZE+31)/32];
};

// 32 bytes
struct ArgInfo {
  BitSet<128> used_;
  BitSet<128> color_;
  int offset_ = 0;

  int used() const { return used_.popcount(); }
  int used(int ARG) const { return used_[ARG]; }
  int next(int ARG) const { return used_.next(ARG); }
  int prev(int ARG) const { return used_.prev(ARG); }
  int nth(int ARG) const { return used_.nth(ARG); }
  int iscolor(int ARG) const { return color_[ARG]; }
  void clear_below_offset() {
    for (int i = 0; i < offset_; i++) {
      used_.clear(i);
    }
  }
  void operator>>=(int bits) {
    if (bits == 0) return;
    used_ >>= bits;
    color_ >>= bits;
  }
};

class GetUsedArgsParser : public ArgParser {
public:
  GetUsedArgsParser(const char* data) : ArgParser(data) {}
  const char* GetArg(int arg_num, const char* name, const char* default_value) override {
    int arg = arg_num + arginfo_.offset_;
    arginfo_.used_.set(arg);
    if (strchr(default_value, ',')) arginfo_.color_.set(arg);
    return ArgParser::GetArg(arg_num, name, default_value);
  }
  void Shift(int words) override {
    ArgParser::Shift(words);
    arginfo_.offset_ += words;
  }
  int used() { return arginfo_.used(); }
  int next(int ARG) { return arginfo_.next(ARG); }
  int prev(int ARG) { return arginfo_.prev(ARG); }
  int nth(int ARG) { return arginfo_.nth(ARG); }
  ArgInfo& getArgInfo() { return arginfo_; }
private:
  ArgInfo arginfo_;
};

class GetMaxArgParser : public ArgParser {
public:
  GetMaxArgParser(const char* data) : ArgParser(data) {}
  const char* GetArg(int arg_num, const char* name, const char* default_value) override {
    max_ = std::max<int>(max_, arg_num + offset_);
    return ArgParser::GetArg(arg_num, name, default_value);
  }
  void Shift(int words) override {
    ArgParser::Shift(words);
    offset_ += words;
  }
  int max_arg() {
    return max_;
  }
private:
  int offset_ = 0;
  int max_ = 0;
};

class DefaultArgumentParserWrapper : public ArgParserInterface {
public:
  DefaultArgumentParserWrapper(ArgParserInterface* ap, const char* defs) : argParser_(ap), defaults_(defs) {}
  const char* GetArg(int arg_num, const char* name, const char* default_value) override {
    const char* d = defaults_.GetArg(arg_num, name, default_value);
    return argParser_->GetArg(arg_num, name, d);
  }
  void Shift(int words) override {
    defaults_.Shift(words);
    argParser_->Shift(words);
  }

  ArgParserInterface* argParser_;
  ArgParser defaults_;
};

extern ArgParserInterface* CurrentArgParser;

#endif
