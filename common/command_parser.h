#ifndef COMMON_COMMAND_PARSER_H
#define COMMON_COMMAND_PARSER_H

#include "linked_list.h"
#include "stdout.h"

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
protected:
  virtual bool Parse(const char* cmd, const char* arg) = 0;
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

// Returns true if a line was found.
// To get the first line, call with previous == nullptr.
template<size_t MAXLINE>
bool RunCommandAndFindNextSortedLine(const char* cmd, const char* arg,
				     const char* previous,
				     char* output,
				     bool reverse) {
  GetNextLineCommandOutput<MAXLINE> output_helper(previous, output, reverse);
  Print* saved_output = stdout_output;
  stdout_output = &output_helper;
  CommandParser::DoParse(cmd, arg);
  stdout_output = saved_output;
  return output_helper.found();
}


#endif

template<size_t bufsize>
class SortedLineHelper {
public:
  explicit SortedLineHelper(const char* cmd) : cmd_(cmd) {}

  int init() {
    InitialPositionFinder finder(*this, get_current_value());
    run();
    num_lines_ = finder.total_;
    current_value_pos_ = finder.num_lower_;
    current_value_exists_ = finder.num_lower_ + finder.num_higher_ + 1 == finder.total_;
    line_.clear();
    //fprintf(stderr,"NUM LINES = %d current=%s current = %d exists=%d\n", num_lines_,
    //get_current_value(),
    // current_value_pos_, current_value_exists_);

    // Menu start position.
    return finder.num_lower_;
  }
  
  class InitialPositionFinder : public CommandOutputCaptureHelper<bufsize> {
  public:
    InitialPositionFinder(SortedLineHelper& helper, const StringPiece current_value) : helper_(helper), current_value_(current_value) {}

    void GotLine(const Line<bufsize>& l) override {
      //fprintf(stderr, "GOTLINE %s\n", l.line);
      total_++;
      if (l > current_value_) num_higher_++;
      else if (l < current_value_) num_lower_++;
    }

    SortedLineHelper& helper_;
    int total_ = 0;
    int num_higher_ = 0;
    int num_lower_ = 0;
    const StringPiece current_value_;
  };
  
  class SortedLineFinder : public CommandOutputCaptureHelper<bufsize> {
  public:
    SortedLineFinder(SortedLineHelper& helper) : helper_(helper), ret_(helper.line_) {}

    void GotLine(const Line<bufsize>& l) override {
      // fprintf(stderr, "GOTLINE %s   ret = %s\n", l.line, ret_.line);
      if (l <= min_) return;
      if (l >= max_) return;
      // fprintf(stderr, "candidate\n");

      if (!ret_) {
	ret_ = l;
      } else if(first_) {
	if (l < ret_) ret_ = l;
      } else {
	if (l > ret_) ret_ = l;
      }
      
      if (!guess_) {
	guess_ = l;
      } else if (l >= guess_) {
	if (num_higher_ == 0 && num_lower_ < wanted_) {
	  guess_ = l;
	  num_lower_++;
	  return;
	}
	num_higher_++;
      } else {
	num_lower_++;
      }
    }

    void find(int n) {
      // fprintf(stderr, "FIND %d\n", n);
      min_.clear();
      max_.line[0] = 127;
      max_.line[1] = 0;
      max_.line_number = helper_.num_lines_ + 1;
      wanted_shift_ = 0;
      
      if (ret_.line_number >= 0) {
	if (ret_.line_number > n) {
	  max_ = ret_;
	} else {
	  min_ = ret_;
	}
      }
      while (ret_.line_number != n) {
	//fprintf(stderr, "MINL=%d(%s) MAXL=%d(%s)\n",
	// min_.line_number, min_.line,
	// max_.line_number, max_.line);
	first_ = n - min_.line_number < max_.line_number - n;
	num_lower_ = 0;
	num_higher_ = 0;
	wanted_ = n - min_.line_number;
	if (wanted_shift_) wanted_shift_--;
	if (wanted_shift_) wanted_ <<= wanted_shift_;
	ret_.clear();
	guess_.clear();
	helper_.run();
	int total = max_.line_number - min_.line_number;
	int expected_highstuff = (total - wanted_) / 2;
	int actual_highstuff = num_lower_ - wanted_;
	if (expected_highstuff > actual_highstuff) {
	  wanted_shift_+=2;
	}

	if (first_) {
	  ret_.line_number = min_.line_number + 1;
	  min_ = ret_;
	} else {
	  ret_.line_number = max_.line_number - 1;
	  max_ = ret_;
	}
	guess_.line_number = min_.line_number + 1 + num_lower_;
	// fprintf(stderr, "guess = %d(%s)\n", guess_.line_number, guess_.line);
	
	if (guess_.line_number > n) {
	  max_ = guess_;
	} else if (guess_.line_number < n) {
	  min_ = guess_;
	  wanted_shift_ = 0;
	} else {
	  ret_ = guess_;
	}
      }
    }

    SortedLineHelper& helper_;
    int num_lower_;
    int num_higher_;
    int wanted_;
    int wanted_shift_;
    bool first_;
    Line<bufsize> min_;
    Line<bufsize> max_;
    Line<bufsize> guess_;
    Line<bufsize>& ret_;
  };

  virtual StringPiece get_current_value() = 0;
  
  const StringPiece get(int n) {
    if (n >= current_value_pos_) {
      if (n == current_value_pos_) {
	return get_current_value();
      }
      if (!current_value_exists_) n--;
    }
    if (n != line_.line_number) {
      SortedLineFinder finder(*this);
      finder.find(n);
    }
    return line_.line;
  }

  void run() {
    // fprintf(stderr, "RUN\n");
    CommandParser::DoParse(cmd_, nullptr);
  }
protected:
  int current_value_pos_;
  bool current_value_exists_;
  int num_lines_;
  Line<bufsize> line_;
  const char* cmd_;
};

#endif
