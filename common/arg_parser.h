#ifndef COMMON_ARG_PARSER_H
#define COMMON_ARG_PARSER_H

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

const char* SkipSpace(const char* str) {
  while (*str == ' ' || *str == '\t') str++;
  return str;
}

const char* SkipWord(const char* str) {
  str = SkipSpace(str);
  while (*str != ' ' && *str != '\t' && *str) str++;
  return str;
}

int CountWords(const char* str) {
  int words = 0;
  while (*str) {
    str = SkipWord(str);
    words++;
  }
  return words;
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
private:
  const char* str_;
};

class ArgParserPrinter : public ArgParserInterface {
public:
  const char* GetArg(int arg_num,
		     const char* name,
		     const char* default_value) override {
    arg_num += offset;
    if (current_arg == arg_num) {
      STDOUT.print(name);
      STDOUT.print(" ");
      STDOUT.println(default_value);
      try_again = true;
      current_arg++;
    }
    return nullptr;
  }
  bool next() {
    offset = 0;
    bool ret = try_again;
    try_again = false;
    return ret;
  }
  
  void Shift(int words) override { offset += words; }
  int offset = 0;
  bool try_again = false;
  int current_arg = 1;
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

class GetMaxArgParser : public ArgParserInterface {
public:
  const char* GetArg(int arg_num,
		     const char* name,
		     const char* default_value) override {
    max_ = std::max<int>(max_, arg_num + offset_);
    return default_value;
  }
  void Shift(int words) override {
    offset_ += words;
  }
  int max_arg() {
    return max_;
  }
private:
  int offset_ = 0;
  int max_ = 0;
};

extern ArgParserInterface* CurrentArgParser;

#endif
