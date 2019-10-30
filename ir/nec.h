#ifndef IR_NEC_H
#define IR_NEC_H

class NECDecoder : public IRDecoderHelper {
protected:
  enum class State {
    BEGIN,
    AWAIT_MARK2OR3,
    REPEAT,
    BITS,
  };
  uint32_t data_ = 0;
  uint8_t num_bits_ = 0;
  State state_ = State::BEGIN;

  const char* name() override { return "BlasterDecoder"; }

  void signal(bool high, IRLength length) override {
#if 0    
    if (state_ != State::BEGIN) {
      STDOUT.print("NEC: ");
      STDOUT.print((int)state_);
      STDOUT.print(high ? " hi " : " lo ");
      STDOUT.print((int)length);
      STDOUT.print(" bits = ");
      STDOUT.println(num_bits_);
    }
#endif    
    switch (state_) {
      case State::BEGIN:
	if (length == IRLength::MARK1 && high) {
	  state_ = State::AWAIT_MARK2OR3;
	  return;
	} else {
	  goto fail;
	}

      case State::AWAIT_MARK2OR3:
	if (high) goto fail;
	switch (length) {
	  case IRLength::MARK3:
	    state_ = State::REPEAT;
	    return;
	  case IRLength::MARK2:
	    state_ = State::BITS;
	    return;
	  default:
	    goto fail;
	}

      case State::REPEAT:
	if (high && length == IRLength::SHORT) {
	  STDOUT.println("REPEAT!");
	}
	goto fail;  // reset

      case State::BITS:
	if (high) {
	  if (length != IRLength::SHORT) goto fail;
	  return;
	}
	switch (length) {
	  case IRLength::LONG:
	    num_bits_++;
	    data_ <<= 1;
	    data_ |= 1;
	    break;
	  case IRLength::SHORT:
	    num_bits_++;
	    data_ <<= 1;
	    break;
	  default:              goto fail;
	}
	if (num_bits_ < 32) return;

	STDOUT.print("NEC CMD ");
	STDOUT.println(data_, HEX);
    }

  fail:
    if (num_bits_ == 16) {
      STDOUT.print("NEC16 CMD ");
      STDOUT.println(data_, HEX);
    }
    state_ = State::BEGIN;
    data_ = 0;
    num_bits_ = 0;
  }
  
  IRLength classify(uint32_t us) override {
    if (us < 560 - 150) return IRLength::ILLEGAL;
    if (us < 560 + 150) return IRLength::SHORT;
    if (us < 1690 - 200) return IRLength::ILLEGAL;
    if (us < 1690 + 200) return IRLength::LONG;
    if (us < 2250 - 200) return IRLength::ILLEGAL;
    if (us < 2250 + 200) return IRLength::MARK3;
    if (us < 4500 - 1000) return IRLength::ILLEGAL;
    if (us < 4500 + 1000) return IRLength::MARK2;
    if (us < 9000 - 1000) return IRLength::ILLEGAL;
    if (us < 9000 + 1000) return IRLength::MARK1;
    return IRLength::ILLEGAL;
  }
};

class NECEncoder {
public:
  void Encode16(IRInterface* output, uint16_t data) {
    output->signal(true, 9000);
    output->signal(false, 4500);
    for (int i = 0; i < 16; i++) {
      output->signal(true, 560);
      output->signal(false, data & 0x8000 ? 1690 : 560);
      data <<= 1;
    }
    output->signal(true, 560);
  }
  void Encode32(IRInterface* output, uint32_t data) {
    output->signal(true, 9000);
    output->signal(false, 4500);
    for (int i = 0; i < 32; i++) {
      output->signal(true, 560);
      output->signal(false, data & 0x80000000U ? 1690 : 560);
      data <<= 1;
    }
    output->signal(true, 560);
  }
  void EncodeRepeat(IRInterface* output) {
    output->signal(true, 9000);
    output->signal(false, 2250);
    output->signal(true, 560);
  }
};

#endif
