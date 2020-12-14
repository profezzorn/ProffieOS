#ifndef COMMON_STDOUT_H
#define COMMON_STDOUT_H

#include "monitoring.h"

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


class CatchCommandOutput : public Print {
 public:
  CatchCommandOutput(int target_line, char* buf, int bufsize) :
    target_line_(target_line),
    buf_(buf),
    bufsize_(bufsize)  {
  }
  size_t write(uint8_t b) override {
    if (b == '\n') { line_++; return 1; }
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

extern ConsoleHelper STDOUT;

#define DEFINE_COMMON_STDOUT_GLOBALS            \
Print* default_output = &Serial;                \
Print* stdout_output = &Serial;                 \
ConsoleHelper STDOUT

#endif
