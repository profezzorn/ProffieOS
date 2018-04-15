#ifndef COMMON_ARG_PARSER_H
#define COMMON_ARG_PARSER_H

class ArgParserInterface {
public:
  virtual const char* GetArg(int arg_num, const char* name) = 0;
};

class ArgParser : public ArgParserInterface {
public:
  ArgParser(const char* data) : str_(data) { }

  const char* GetArg(int arg_num, const char* name) override {
    const char* ret = str_;
    int arg = 0;
    while (true) {
      while (*ret == ' ' || *ret == '\t') ret++;
      if (!*ret) {
	STDOUT.print("Missing argument ");
	STDOUT.println(arg_num);
	return nullptr;
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
  const char* GetArg(int arg_num, const char* name) override {
    if (current_arg == arg_num) {
      STDOUT.print(" ");
      STDOUT.print(name);
      try_again = true;
    }
    return nullptr;
  }
  bool next() {
    bool ret = try_again;
    try_again = false;
    current_arg++;
    return ret;
  }
  
  bool try_again = false;
  int current_arg = 0;
};

extern ArgParserInterface* ArgParser;

#endif
