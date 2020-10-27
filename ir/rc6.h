#ifndef IR_RC6_H
#define IR_RC6_H

class RC6Decoder : public IRDecoder {
protected:
  uint32_t data_;
  uint8_t pos_;
  bool toggle_;

  // RC6 mode 0
  // 0 = LOW, then HIGH
  // 1 = HIGH then LOW
  // POS data    
  //  0  leader  HIGH
  //  1  leader  HIGH
  //  2  leader  HIGH
  //  3  leader  HIGH
  //  4  leader  HIGH
  //  5  leader  HIGH
  //  6  leader  LOW
  //  7  leader  LOW
  //  8  start bit  (always 1)
  //  9  start bit
  // 10  mode bit 2
  // 11  mode bit 2
  // 12  mode bit 1
  // 13  mode bit 1
  // 14  mode bit 0
  // 15  mode bit 0
  // 16  toggle  (double-length bit)
  // 17  toggle
  // 18  toggle
  // 19  toggle
  // 20  ctrl bit 7
  // 21  ctrl bit 7
  // 22  ctrl bit 6
  // 23  ctrl bit 6
  // 24  ctrl bit 5
  // 25  ctrl bit 5
  // 26  ctrl bit 4
  // 27  ctrl bit 4
  // 28  ctrl bit 3
  // 29  ctrl bit 3
  // 30  ctrl bit 2
  // 31  ctrl bit 2
  // 32  ctrl bit 1
  // 33  ctrl bit 1
  // 34  ctrl bit 0
  // 35  ctrl bit 0
  // 36  info bit 7
  // 37  info bit 7
  // 38  info bit 6
  // 39  info bit 6
  // 40  info bit 5
  // 41  info bit 5
  // 42  info bit 4
  // 43  info bit 4
  // 44  info bit 3
  // 45  info bit 3
  // 46  info bit 2
  // 47  info bit 2
  // 48  info bit 1
  // 49  info bit 1
  // 50  info bit 0
  // 51  info bit 0
  // 52  LOW for a few ms

  const char* name() override { return "RC6Decoder"; }

  void signal(bool high, uint32_t length) override {
    uint32_t offset = length % 444;
    uint32_t steps = (length + 222) / 444;
    uint32_t tolerance = 150;
#if 0    
    if (length < 10000 && length > 300) {
      STDOUT.print("RC6: ");
      STDOUT.print(pos_);
      STDOUT.print(high ? " hi " : " lo ");
      STDOUT.print((int)length);
      STDOUT.print(" length= ");
      STDOUT.print(length);
      STDOUT.print(" offset= ");
      STDOUT.print(offset);
      STDOUT.print(" steps= ");
      STDOUT.println(steps);
    }
#endif
    if (offset > tolerance && offset < 444 - tolerance) goto fail;
    while (steps > 0) {
      switch (pos_) {
	case 0:
	  if (steps != 6 || !high) goto fail;
	  pos_ += 6;
	  steps = 0;
	  break;

	case 6:
	  if (steps != 2 || high) goto fail;
	  pos_ += steps;
	  steps = 0;
	  break;

	case 16:
	  if (steps != 2) goto fail;
	  toggle_ = high;
	  steps = 0;
	  pos_ += 2;
	  break;

	case 18:
	  if (steps < 2) goto fail;
	  if (high == toggle_) goto fail;
	  steps -= 2;
	  pos_ += 2;
	  break;

	default:
	  if ((pos_ & 1) == 0) {
	    if (steps != 1) goto fail;
	    data_ <<= 1;
	    if (high) data_ |= 1;
	    pos_++;
	    steps = 0;
	  } else {
	    if (((data_ & 1) == 1) == high) goto fail;
	    steps--;
	    pos_++;
	    if (pos_ >= 52) {
	      uint8_t ctrl = data_ >> 8;
	      uint8_t info = data_;
	      uint8_t mode = (data_ >> 16) & 0x7;
	      uint8_t start = data_ >> 19;
	      if (start && mode == 0) {
		STDOUT.print("RC6 ");
		STDOUT.print(ctrl, HEX);
		STDOUT.print(" ");
		STDOUT.println(info, HEX);
	      }
	      goto fail;
	    }
	  }
      }
    }
    return;
    
    fail:
      pos_ = 0;
      data_ = 0;
      toggle_ = false;
  }
};

class RC6Encoder {
public:
  void EncodeBit(IRInterface* output, bool bit) {
    output->signal(bit, 444);
    output->signal(!bit, 444);
  }
  void EncodeSlowBit(IRInterface* output, bool bit) {
    output->signal(bit, 889);
    output->signal(!bit, 889);
  }
  void EncodeByte(IRInterface* output, uint8_t x) {
    for (int i = 0; i < 8; i++, x<<=1) {
      EncodeBit(output, (x & 0x80) == 0x80);
    }
  }
  void EncodeRC6mode0(IRInterface* output, bool toggle, uint8_t ctrl, uint8_t info) {
    output->signal(true, 444 * 6);
    output->signal(false, 444 * 2);
    EncodeBit(output, true);
    EncodeBit(output, false);
    EncodeBit(output, false);
    EncodeBit(output, false);
    EncodeSlowBit(output, toggle);
    EncodeByte(output, ctrl);
    EncodeByte(output, info);
  }
};

#endif
