#ifndef COMMON_STDOUT_H
#define COMMON_STDOUT_H

#include "monitoring.h"

extern Print* default_output;
extern Print* stdout_output;

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
  // TODO: use SFINAE to avoid print problems.
  template<class X> void out(X& v) { v.printTo(*this); }
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

extern ConsoleHelper STDOUT;

#define DEFINE_COMMON_STDOUT_GLOBALS            \
Print* default_output = &Serial;                \
Print* stdout_output = &Serial;                 \
ConsoleHelper STDOUT

#endif
