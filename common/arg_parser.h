#ifndef COMMON_ARG_PARSER_H
#define COMMON_ARG_PARSER_H

class ArgParserInterface {
public:
  virtual const char* GetArg(int arg_num,
			     const char* name,
			     const char* default_value) = 0;
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

const char* SkipWord(const char* str) {
  while (*str == ' ' || *str == '\t') str++;
  while (*str != ' ' && *str != '\t' && *str) str++;
  return str;
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
      if (!*ret) {
	STDOUT.print("Missing argument ");
	STDOUT.println(arg_num);
	return default_value;
      }
      if (++arg == arg_num) return ret;
      while (*ret && *ret != ' ' && *ret != '\t') ret++;
    }
  }
private:
  const char* str_;
};

class ArgParserPrinter : public ArgParserInterface {
public:
  const char* GetArg(int arg_num,
		     const char* name,
		     const char* default_value) override {
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
    bool ret = try_again;
    try_again = false;
    return ret;
  }
  
  bool try_again = false;
  int current_arg = 1;
};

extern ArgParserInterface* CurrentArgParser;

#endif
