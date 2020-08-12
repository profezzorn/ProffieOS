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
  enum Codes : uint32_t {
    TIVO_TIVO     = 0xA10CF00F,
    TIVO_TV_POWER = 0xA10C0807,
    TIVO_TV_INPUT = 0xA10C2C03,
    TIVO_WINDOW   = 0xA10C220D,
    TIVO_LIVE_TV  = 0xA10C8807,
    TIVO_INFO     = 0xA10CC807,
    TIVO_GUIDE    = 0xA10C6C03,
    TIVO_UP       = 0xA10C2807,
    TIVO_LEFT     = 0xA10CE807,
    TIVO_RIGHT    = 0xA10CA807,
    TIVO_DOWN     = 0xA10C6807,
    TIVO_SELECT   = 0xA10C9807,
    TIVO_TH_DOWN  = 0xA10C1807,
    TIVO_TH_UP    = 0xA10C5807,
    TIVO_VOL_UP   = 0xA10C3807,
    TIVO_VOL_DOWN = 0xA10CB807,
    TIVO_MUTE     = 0xA10CD807,
    TIVO_RECORD   = 0xA10C040B,
    TIVO_PG_UP    = 0xA10C7807,
    TIVO_PG_DOWN  = 0xA10CF807,
    TIVO_PLAY     = 0xA10C840B,
    TIVO_REWIND   = 0xA10C440B,
    TIVO_PAUSE    = 0xA10CC40B,
    TIVO_FORWARD  = 0xA10C240B,
    TIVO_SLOW     = 0xA10CA40B,
    TIVO_REPLAY   = 0xA10C640B,
    TIVO_JUMP     = 0xA10CE40B,
    TIVO_1        = 0xA10C140B,
    TIVO_2        = 0xA10C940B,
    TIVO_3        = 0xA10C540B,
    TIVO_4        = 0xA10CD40B,
    TIVO_5        = 0xA10C340B,
    TIVO_6        = 0xA10CB40B,
    TIVO_7        = 0xA10C740B,
    TIVO_8        = 0xA10CF40B,
    TIVO_9        = 0xA10C0C03,
    TIVO_0        = 0xA10C8C03,
    TIVO_CLEAR    = 0xA10C4C03,
    TIVO_EXIT     = 0xA10CA20D,
    TIVO_ENTER    = 0xA10CCC03,

    TIVO_UNIT_1   = 0x00000080,
    TIVO_UNIT_2   = 0x00000040,
    TIVO_UNIT_3   = 0x000000C0,
    TIVO_UNIT_4   = 0x00000020,
    TIVO_UNIT_5   = 0x000000A0,
    TIVO_UNIT_6   = 0x00000060,
    TIVO_UNIT_7   = 0x000000E0,
    TIVO_UNIT_8   = 0x00000010,
    TIVO_UNIT_9   = 0x00000090,
  };
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
