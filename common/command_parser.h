#ifndef COMMON_COMMAND_PARSER_H
#define COMMON_COMMAND_PARSER_H

#include "linked_list.h"

// Command parsing linked list base class.
class CommandParser;
extern CommandParser* parsers;

class CommandParser {
public:
  void Link() {
    CHECK_LL(CommandParser, parsers, next_parser_);
    next_parser_ = parsers;
    parsers = this;
    CHECK_LL(CommandParser, parsers, next_parser_);
  }
  void Unlink() {
    CHECK_LL(CommandParser, parsers, next_parser_);
    for (CommandParser** i = &parsers; *i; i = &(*i)->next_parser_) {
      if (*i == this) {
        *i = next_parser_;
    CHECK_LL(CommandParser, parsers, next_parser_);
        return;
      }
    }
    CHECK_LL(CommandParser, parsers, next_parser_);
  }

  CommandParser() { Link(); }
  explicit CommandParser(NoLink _) {}
  ~CommandParser() { Unlink(); }
  static bool DoParse(const char* cmd, const char* arg) {
    CHECK_LL(CommandParser, parsers, next_parser_);
    for (CommandParser *p = parsers; p; p = p->next_parser_) {
      if (p->Parse(cmd, arg))
        return true;
    }
    return false;
  }
  static void DoHelp() {
    CHECK_LL(CommandParser, parsers, next_parser_);
    for (CommandParser *p = parsers; p; p = p->next_parser_) {
      p->Help();
    }
  }
protected:
  virtual bool Parse(const char* cmd, const char* arg) = 0;
  virtual void Help() = 0;
private:
  CommandParser* next_parser_;
};

// This ifdef avoids problems with tests
#ifdef DEFINE_COMMON_STDOUT_GLOBALS

int RunCommandAndGetSingleLine(const char* cmd, const char* arg,
			       int target_line, char* buf, int bufsize) {
  CatchCommandOutput output_helper(target_line, buf, bufsize);
  Print* saved_output = stdout_output;
  stdout_output = &output_helper;
  CommandParser::DoParse(cmd, arg);
  stdout_output = saved_output;
  return output_helper.num_lines();
}

#endif

#endif
