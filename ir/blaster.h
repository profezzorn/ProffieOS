#ifndef IR_BLASTER_H
#define IR_BLASTER_H

class BlasterDecoder : public IRDecoderHelper {
protected:
  enum class State {
    Start,
    Bits,
  };
  uint32_t data_;
  uint8_t num_bits_;
  State state_;

  const char* name() override { return "BlasterDecoder"; }

  void signal(bool high, IRLength length) override {
    switch (state_) {
      case State::Start:
	data_ = 0;
	num_bits_ = 0;
	if (high && length == IRLength::MARK1) state_ = State::Bits;
	return;

      case State::Bits:
#if 0
      STDOUT.print("BLASTER: ");
      STDOUT.print(num_bits_);
      STDOUT.print(high ? " hi " : " lo ");
      STDOUT.print(" length= ");
      STDOUT.println((int)length);
#endif
	data_ <<= 1;
	switch (length) {
	  case IRLength::LONG:
	    data_ |= 1;
	  case IRLength::SHORT:
	    break;
	  default:
	    state_ = State::Start;
	    return;
	}
	switch (++num_bits_) {
	  case 16:
	    STDOUT.print("DATA SO FAR: ");
	    STDOUT.println(data_, HEX);
	    if (data_ != 0x00AA) state_ = State::Start;
	    return;

	  case 40:
	    uint8_t team = (data_ >> 16);
	    uint8_t damage = (data_ >> 8);
	    uint8_t parity = data_;
	    uint8_t computed_parity = (team + damage) + ((team + damage - 1) / 5 + 4);
	    STDOUT.print(" TEAM: ");
	    STDOUT.print(team);
	    STDOUT.print(" DAMAGE: ");
	    STDOUT.print(damage);
	    STDOUT.print(" PARITY: ");
	    STDOUT.print(parity);
	    STDOUT.print(" computed parity: ");
	    STDOUT.println(computed_parity);
	    if (parity == computed_parity) {
	      // Don't check the parity here, because it fails to often.
	      // We can still be fairly certain that something happened.
	      // TODO: Route through prop and implement full laser tag game.
	      if (SaberBase::IsOn()) {
		SaberBase::DoBlast();
	      }
	    }
	    state_ = State::Start;
	    return;
	}
    }
  }
  
  IRLength classify(uint32_t us) override {
    if (us < 400 - 200) return IRLength::ILLEGAL;
    if (us < 400 + 200) return IRLength::SHORT;
    if (us < 800 - 200) return IRLength::ILLEGAL;
    if (us < 800 + 200) return IRLength::LONG;
    if (us < 1600 - 200) return IRLength::ILLEGAL;
    if (us < 1600 + 200) return IRLength::MARK1;
    return IRLength::ILLEGAL;
  }
};

class BlasterEncoder {
public:
  void EncodeByte(IRInterface* output, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
      output->signal((i & 1) == 1, byte & 0x80 ? 800 : 400);
      byte <<= 1;
    }
  }
  void Encode(IRInterface* output, uint8_t team, uint8_t damage) {
    output->signal(true, 1700);
    EncodeByte(output, 0);
    EncodeByte(output, 0xAA);
    EncodeByte(output, team);
    EncodeByte(output, damage);
    EncodeByte(output, (team + damage) + ((team + damage - 1) / 5 + 4));
  }
};

class BlasterIRSender : public BlasterEncoder, public SaberBase {
public:
  void SB_Effect(EffectType effect, float location) {
    IRSender* sender;
    switch (effect) {
      case EFFECT_FIRE:
	sender= GetIRSender();
	Encode(sender, 1 /* team */, 3  /* damage */);
	sender->send();
	return;
	
      case EFFECT_STUN:
	sender = GetIRSender();
	Encode(sender, 1 /* team */, 1 /* damage */);
	sender->send();
	return;

      default: break;
    }
  }
};

#endif
