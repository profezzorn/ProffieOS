#ifndef IR_IR_H
#define IR_IR_H

class IRInterface {
public:
  virtual void signal(bool high, uint32_t us) = 0;
};

class IRSender : public IRInterface {
public:
  virtual void send() = 0;
};

IRSender* GetIRSender();

class IRDecoder;
IRDecoder* ir_decoders = nullptr;

class IRDecoder : public IRInterface {
public:
  IRDecoder() {
    next_decoder_ = ir_decoders;
    ir_decoders = this;
  }
  static void DoSignal(bool high, uint32_t us) {
    for (IRDecoder *d = ir_decoders; d; d = d->next_decoder_) {
      d->signal(high, us);
    }
  }
protected:
  virtual const char* name() = 0;
private:
  IRDecoder* next_decoder_;
};

class IRDecoderHelper : public IRDecoder {
protected:
  enum class IRLength {
    ILLEGAL,
    SHORT,
    LONG,
    MARK1,
    MARK2,
    MARK3
  };

  void signal(bool high, uint32_t us) override {
    signal(high, classify(us));
  }

protected:
  virtual void signal(bool high, IRLength len) = 0;
  virtual IRLength classify(uint32_t us) = 0;
};

#endif
  
