#ifndef IR_PRINT_H
#define IR_PRINT_H

class PrintDecoder : public IRDecoder {
public:
  const char* name() override { return "PrintDecoder"; }
  
  void signal(bool high, uint32_t length) override {
    bool valid = length > 300 && length < 6000;
    if (valid || last_valid_) {
      STDOUT.print("IR: ");
      STDOUT.print(high ? "hi" : "lo");
      STDOUT.print(" for ");
      STDOUT.print(length);
      STDOUT.println(" us");
    }
    last_valid_ = valid;
  }
private:
  bool last_valid_ = false;
};

#endif
