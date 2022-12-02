#ifndef COMMON_STDOUT_H
#define COMMON_STDOUT_H

#include "monitoring.h"

#ifdef PROFFIE_TEST
struct Print {
  void print(const char* s) { write((const uint8_t*)s, strlen(s)); }
  void print(float v) {
    char tmp[64];
    sprintf(tmp, "%f", v);
    print(tmp);
  }
  void print(int v, int base) {
    char tmp[64];
    sprintf(tmp, "%d", v);
    print(tmp);
  }
  void write(char s) { write( (uint8_t) s); }
  template<class T>
  void println(T s) { print(s); write('\n'); }
  template<class T>
  void println(T s, int base) { print(s); write('\n'); }
  virtual size_t write(uint8_t s) { putchar(s); return 1; }
  virtual size_t write(const uint8_t *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) write(buffer[i]);
    return size;
  }

};
#endif

extern Print* default_output;
extern Print* stdout_output;

template<typename T, typename X = void> struct PrintHelper {
  static void out(Print& p, T& x) { p.print(x); }
};

template<typename T> struct PrintHelper<T, decltype(((T*)0)->printTo(*(Print*)0))> {
  static void out(Print& p, T& x) { x.printTo(p); }
};


class ConsoleHelper : public Print {
public:
  bool debug_is_on() const {
    return monitor.IsMonitoring(Monitoring::MonitorSerial) &&
      stdout_output != default_output;
  }
  size_t write(uint8_t b) override {
    size_t ret = stdout_output->write(b);
    if (debug_is_on()) default_output->write(b);
    return ret;
  }
  size_t write(const uint8_t *buffer, size_t size) override {
    size_t ret = stdout_output->write(buffer, size);
    if (debug_is_on()) default_output->write(buffer, size);
    return ret;
  }
  template<typename T>
  ConsoleHelper& operator<<(T v) {
    PrintHelper<T>::out(*this, v);
    return *this;
  }

#ifdef TEENSYDUINO
  int availableForWrite(void) override {
    return stdout_output->availableForWrite();
  }
  virtual void flush() override {
    stdout_output->flush();
    if (debug_is_on()) default_output->flush();
  }
#endif
};

template<typename T>
Print& operator<<(Print& p, T v) {
  PrintHelper<const T>::out(p, v);
  return p;
}

class CatchCommandOutput : public Print {
 public:
  CatchCommandOutput(int target_line, char* buf, int bufsize) :
    target_line_(target_line),
    buf_(buf),
    bufsize_(bufsize)  {
  }
  size_t write(uint8_t b) override {
    if (b == '\n') { line_++; return 1; }
    if (b == '\r') return 1;	  
    if (line_ == target_line_) {
      if (bufsize_ > 1) {
	*buf_ = b;
	buf_++;
	*buf_ = 0;
	bufsize_--;
      }
    }
    return 1;
  }
 int num_lines() { return line_ - 1; } // assume last line is empty
private:
 int target_line_ = 0;
 int line_ = 1;

 char* buf_;
 size_t bufsize_;
};

template<size_t MAXLINE>
class GetNextLineCommandOutput : public Print {
 public:
  GetNextLineCommandOutput(const char* previous, char* target_buf, bool reverse) :
    previous_(previous),
    buf_(target_buf),
    reverse_(reverse) {
  }

  bool compare(const char* a, const char *b) const {
    return reverse_ ? (strcmp(a, b) > 0) : (strcmp(a, b) < 0);
  }
  size_t write(uint8_t b) override {
    if (b == '\n') {
      if (pos_) {
	line_[pos_] = 0;
	if (previous_ == nullptr || compare(previous_, line_)) {
	  if (!found_ || compare(line_, buf_)) {
	    strcpy(buf_, line_);
	    found_ = true;
	  }
	}
	pos_ = 0;
      }
      return 1;
    }
    if (b == '\r') return 1;
    if (pos_ <  MAXLINE - 1) line_[pos_++] = b;
    return 1;
  }
  bool found() const { return found_; }
private:
  bool reverse_;
  bool found_ = false;
  size_t pos_ = 0;
  char* buf_;
  const char* previous_;
  char line_[MAXLINE];
};

extern ConsoleHelper STDOUT;

#define DEFINE_COMMON_STDOUT_GLOBALS            \
Print* default_output = &Serial;                \
Print* stdout_output = &Serial;                 \
ConsoleHelper STDOUT
#define STDERR (*default_output)

#endif
