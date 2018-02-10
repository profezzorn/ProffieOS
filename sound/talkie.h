#ifndef SOUND_TALKIE_H
#define SOUND_TALKIE_H

// This is adapted from the Talkie Library

#include "voice_data.h"

// TMS5200 coefficients
// These values are derived from the MAME project.
// See http://mamedev.org/source/src/emu/sound/tms5110r.c.html
//
const uint8_t tmsEnergy[0x10] = {
  0x00,0x01,0x02,0x03,0x04,0x06,0x08,0x0b,
  0x10,0x17,0x21,0x2f,0x3f,0x55,0x72,0x00
};
const uint8_t tmsPeriod[0x40] = {
  0x00,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
  0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,
  0x1d,0x1e,0x1f,0x20,0x22,0x24,0x26,0x28,
  0x29,0x2b,0x2d,0x30,0x31,0x33,0x36,0x37,
  0x39,0x3c,0x3e,0x40,0x44,0x48,0x4a,0x4c,
  0x51,0x55,0x57,0x5a,0x60,0x63,0x67,0x6b,
  0x70,0x75,0x7a,0x7f,0x85,0x8b,0x91,0x97,
  0x9d,0xa4,0xab,0xb2,0xba,0xc2,0xca,0xd3
};
// Coefficients below are shifted left 6 bits and in 2's complement form.
const uint16_t tmsK1[0x20] = {
  0x82c0,0x8380,0x8440,0x8580,0x86c0,0x8880,0x8ac0,0x8d40,
  0x9080,0x9440,0x9900,0x9ec0,0xa580,0xad40,0xb640,0xc0c0,
  0xcc40,0xd900,0xe680,0xf4c0,0x0340,0x1180,0x1f80,0x2cc0,
  0x3900,0x4400,0x4dc0,0x5640,0x5d80,0x63c0,0x6900,0x6d40
};
const uint16_t tmsK2[0x20] = {
  0xa200,0xa6c0,0xac40,0xb200,0xb880,0xbf80,0xc740,0xcf40,
  0xd7c0,0xe100,0xea40,0xf3c0,0xfd80,0x0740,0x1100,0x1a80,
  0x23c0,0x2c80,0x3500,0x3cc0,0x4400,0x4ac0,0x5100,0x5680,
  0x5b80,0x6000,0x6400,0x6780,0x6ac0,0x6d80,0x7000,0x7e80
};
// Coefficients below are shifted right 2 bits and in 2's complement form.
// Note that 2 bits of precision are lost, but 8 bit multiplicands are
// faster to compute.
const uint8_t tmsK3[0x10] = {
  0x9a,0xa1,0xa9,0xb2,0xbd,0xca,0xd8,0xe7,
  0xf6,0x06,0x16,0x25,0x34,0x40,0x4c,0x55
};
const uint8_t tmsK4[0x10] = {
  0xb8,0xc1,0xcc,0xd7,0xe4,0xf1,0xfe,0x0b,
  0x18,0x25,0x31,0x3c,0x46,0x4e,0x56,0x5d
};
const uint8_t tmsK5[0x10] = {
  0xb1,0xb9,0xc3,0xce,0xd9,0xe5,0xf2,0xff,
  0x0c,0x19,0x26,0x31,0x3c,0x46,0x4e,0x56
};
const uint8_t tmsK6[0x10] = {
  0xd0,0xda,0xe5,0xf0,0xfb,0x07,0x12,0x1d,
  0x28,0x32,0x3b,0x44,0x4b,0x53,0x59,0x5e
};
const uint8_t tmsK7[0x10] = {
  0xc1,0xca,0xd3,0xdd,0xe8,0xf3,0xfe,0x09,
  0x14,0x1f,0x29,0x33,0x3c,0x45,0x4c,0x53
};
const uint8_t tmsK8[0x08] = {
  0xcd,0xe4,0xfe,0x17,0x2f,0x43,0x54,0x61
};
const uint8_t tmsK9[0x08] = {
  0xc2,0xd2,0xe5,0xf8,0x0c,0x20,0x32,0x41
};
const uint8_t tmsK10[0x08] = {
  0xd1,0xdf,0xee,0xfe,0x0d,0x1d,0x2b,0x39
};

#define CHIRP_SIZE 41
const uint8_t chirp[CHIRP_SIZE] = {0x00,0x2a,0xd4,0x32,0xb2,0x12,0x25,0x14,0x02,0xe1,0xc5,0x02,0x5f,0x5a,0x05,0x0f,0x26,0xfc,0xa5,0xa5,0xd6,0xdd,0xdc,0xfc,0x25,0x2b,0x22,0x21,0x0f,0xff,0xf8,0xee,0xed,0xef,0xf7,0xf6,0xfa,0x00,0x03,0x02,0x01};

class Talkie : public AudioStream {
public:
  const uint8_t* words[30];
  size_t num_words = 0;

  void Say(const uint8_t* addr) {
    digitalWrite(amplifierPin, HIGH); // turn on the amplifier
    noInterrupts();
    if (ptrAddr) {
      if (num_words < NELEM(words)) {
        words[num_words++] = addr;
      }
    } else {
      count_ = 199;
      ptrAddr = addr;
      ptrBit = 0;
    }
    interrupts();
  }

  void SayDigit(int digit) {
    switch (digit) {
      case 0: Say(spZERO); break;
      case 1: Say(spONE); break;
      case 2: Say(spTWO); break;
      case 3: Say(spTHREE); break;
      case 4: Say(spFOUR); break;
      case 5: Say(spFIVE); break;
      case 6: Say(spSIX); break;
      case 7: Say(spSEVEN); break;
      case 8: Say(spEIGHT); break;
      case 9: Say(spNINE); break;
    }
  }

  // The ROMs used with the TI speech were serial, not byte wide.
  // Here's a handy routine to flip ROM data which is usually reversed.
  static uint8_t rev(uint8_t a) {
    // 76543210
    a = (a>>4) | (a<<4); // Swap in groups of 4
    // 32107654
    a = ((a & 0xcc)>>2) | ((a & 0x33)<<2); // Swap in groups of 2
    // 10325476
    a = ((a & 0xaa)>>1) | ((a & 0x55)<<1); // Swap bit pairs
    // 01234567
    return a;
  }

  uint8_t getBits(uint8_t bits) {
    uint8_t value;
    uint16_t data;
    data = rev(pgm_read_byte(ptrAddr))<<8;
    if (ptrBit+bits > 8) {
      data |= rev(pgm_read_byte(ptrAddr+1));
    }
    data <<= ptrBit;
    value = data >> (16-bits);
    ptrBit += bits;
    if (ptrBit >= 8) {
      ptrBit -= 8;
      ptrAddr++;
    }
    return value;
  }

  void ReadFrame() {
    if (!ptrAddr) return;
    
    // Read speech data, processing the variable size frames.
    uint8_t energy = getBits(4);
    if (energy == 0) {
      // Energy = 0: rest frame
      synthEnergy = 0;
    } else if (energy == 0xf) {
      // Energy = 15: stop frame. Silence the synthesiser.
      synthEnergy = 0;
      synthK1 = 0;
      synthK2 = 0;
      synthK3 = 0;
      synthK4 = 0;
      synthK5 = 0;
      synthK6 = 0;
      synthK7 = 0;
      synthK8 = 0;
      synthK9 = 0;
      synthK10 = 0;
      if (num_words) {
        ptrAddr = words[0];
        num_words--;
        for (size_t i = 0; i < num_words; i++) words[i] = words[i + 1];
        ptrBit = 0;
      } else {
        ptrAddr = NULL;
      }
    } else {
      synthEnergy = tmsEnergy[energy];
      uint8_t repeat = getBits(1);
      synthPeriod = tmsPeriod[getBits(6)];
      // A repeat frame uses the last coefficients
      if (!repeat) {
        // All frames use the first 4 coefficients
        synthK1 = tmsK1[getBits(5)];
        synthK2 = tmsK2[getBits(5)];
        synthK3 = tmsK3[getBits(4)];
        synthK4 = tmsK4[getBits(4)];
        if (synthPeriod) {
          // Voiced frames use 6 extra coefficients.
          synthK5 = tmsK5[getBits(4)];
          synthK6 = tmsK6[getBits(4)];
          synthK7 = tmsK7[getBits(4)];
          synthK8 = tmsK8[getBits(3)];
          synthK9 = tmsK9[getBits(3)];
          synthK10 = tmsK10[getBits(3)];
        }
      }
    }
  }
  
  int16_t Get8kHz() {
    if (count_++ >= 200) {
      ReadFrame();
      count_ = 0;
    }
    
    static uint8_t periodCounter;
    static int16_t x0,x1,x2,x3,x4,x5,x6,x7,x8,x9;
    int16_t u0,u1,u2,u3,u4,u5,u6,u7,u8,u9,u10;

    if (synthPeriod) {
      // Voiced source
      if (periodCounter < synthPeriod) {
        periodCounter++;
      } else {
        periodCounter = 0;
      }
      if (periodCounter < CHIRP_SIZE) {
        u10 = ((chirp[periodCounter]) * (uint32_t) synthEnergy) >> 8;
      } else {
        u10 = 0;
      }
    } else {
      // Unvoiced source
      static uint16_t synthRand = 1;
      synthRand = (synthRand >> 1) ^ ((synthRand & 1) ? 0xB800 : 0);
      u10 = (synthRand & 1) ? synthEnergy : -synthEnergy;
    }
    // Lattice filter forward path
    u9 = u10 - (((int16_t)synthK10*x9) >> 7);
    u8 = u9 - (((int16_t)synthK9*x8) >> 7);
    u7 = u8 - (((int16_t)synthK8*x7) >> 7);
    u6 = u7 - (((int16_t)synthK7*x6) >> 7);
    u5 = u6 - (((int16_t)synthK6*x5) >> 7);
    u4 = u5 - (((int16_t)synthK5*x4) >> 7);
    u3 = u4 - (((int16_t)synthK4*x3) >> 7);
    u2 = u3 - (((int16_t)synthK3*x2) >> 7);
    u1 = u2 - (((int32_t)synthK2*x1) >> 15);
    u0 = u1 - (((int32_t)synthK1*x0) >> 15);

    // Output clamp
    if (u0 > 511) u0 = 511;
    if (u0 < -512) u0 = -512;

    // Lattice filter reverse path
    x9 = x8 + (((int16_t)synthK9*u8) >> 7);
    x8 = x7 + (((int16_t)synthK8*u7) >> 7);
    x7 = x6 + (((int16_t)synthK7*u6) >> 7);
    x6 = x5 + (((int16_t)synthK6*u5) >> 7);
    x5 = x4 + (((int16_t)synthK5*u4) >> 7);
    x4 = x3 + (((int16_t)synthK4*u3) >> 7);
    x3 = x2 + (((int16_t)synthK3*u2) >> 7);
    x2 = x1 + (((int32_t)synthK2*u1) >> 15);
    x1 = x0 + (((int32_t)synthK1*u0) >> 15);
    x0 = u0;

    return u0 << 5;
  }

  // Very very stupid upsamler, slightly better than
  // just repeating samples.
  int16_t tmp = 0;
  int16_t last = 0;
  int32_t N = 0;
  int16_t Get44kHz() {
    N += 80;
    if (N > 441) {
      last = Get8kHz();
      N -= 441;
    }
    tmp = (tmp * 3 + last) >> 2;
    return tmp;
  }
  
  int read(int16_t* data, int elements) override {
    for (int i = 0; i < elements; i++) {
      data[i] = Get44kHz();
    }
    return elements;
  }
  bool eof() const override {
    return ptrAddr == NULL && tmp == 0;
  }
  bool isPlaying() const {
    return !eof();
  }
  void Stop() override {}

private:
  const uint8_t * ptrAddr = NULL;
  uint8_t ptrBit = 0;
  
  uint8_t synthPeriod = 0;
  uint16_t synthEnergy = 0;
  int16_t synthK1 = 0;
  int16_t synthK2 = 0;
  int8_t synthK3 = 0;
  int8_t synthK4 = 0;
  int8_t synthK5 = 0;
  int8_t synthK6 = 0;
  int8_t synthK7 = 0;
  int8_t synthK8 = 0;
  int8_t synthK9 = 0;
  int8_t synthK10 = 0;

  uint8_t count_ = 0;
  uint8_t pos_ = 0;
};

#endif
