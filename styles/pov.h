#ifndef STYLES_POV_H
#define STYLES_POV_H

// TODO(hubbe): Make it work with FastLED
#if defined(ENABLE_WS2811)

// Usage: &style_pov or StylePtr<StylePtr>
// return value: suitable for preset array

// This style draws images in the air if you swing the saber
// carefully back and forth. Unfortunately you can't really see
// the result without a slow exposure camera. It's possible that
// dotstar APA102 pixels would be fast enough to do the effect
// full justice, but I haven't tried that.

#ifdef POV_INCLUDE_FILE
#include POV_INCLUDE_FILE
#else
#include "star_wars_logo_pov_data.h"
#endif

void rle_decode(const unsigned char *input,
                 unsigned char *output,
                 int output_length) {
  int olen = 0;
  while (olen < output_length) {
    if (*input == 255) {
      int i;
      int offset = input[1]+1;
      int len = input[2];
      input += 3;
      for (i = 0; i < len; i++) {
         *output = output[-offset];
         output++;
         olen++;
      }
    }
    else if (*input < 128) {
      memcpy(output, input+1, *input + 1);
      output += *input + 1;
      olen += *input + 1;
      input += *input + 2;
    } else {
      memset(output, input[1], *input - 128 + 2);
      output += *input - 128 + 2;
      olen += *input - 128 + 2;
      input += 2;
    }
  }
}

class POVStyleBase {
public:
  void run(BladeBase* blade) {
    num_leds_ = blade->num_leds();
  }
  void RenderColumn(int col) {
    rle_decode(imagedata + imageoffsets[col],
	       (unsigned char *)&buffer_, sizeof(buffer_));
  }
  SimpleColor getColor(int led) {
    Color16 c;
#ifdef POV_RGB
    c = buffer_[led * POV_DATA_HEIGHT / num_leds_];
#elif defined(POV_8BIT)
    c = pov_color_map[buffer_[led * POV_DATA_HEIGHT / num_leds_]];
#else
    c = image_color * buffer_[led * POV_DATA_HEIGHT / num_leds_];
#endif
    return SimpleColor(c);
  }
private:
  int num_leds_;

#ifdef POV_RGB
  Color8 buffer_[POV_DATA_HEIGHT];
#else
  uint8_t buffer_[POV_DATA_HEIGHT];
#endif
};

// POV writer.
template<int MIN_DEGREES=-45, int MAX_DEGREES=45, bool REPEAT=false, bool MIRROR=true>
class StylePOV : public POVStyleBase {
  public:
  void run(BladeBase* blade) {
    float degrees = fusor.pov_angle() * 180.0 / M_PI;
    float fraction = (degrees - MIN_DEGREES) / (MAX_DEGREES - MIN_DEGREES);
    if (REPEAT) {
      if (MIRROR) {
	fraction = fract(fraction / 2.0) * fraction;
	if (fraction > 1.0) {
	  fraction = 2.0 - fraction;
	}
      } else {
	fraction = fract(fraction);
      }
    } else {
      if (fraction < 0 || fraction > 1.0) {
	blade->clear();
	return;
      }
    }
    POVStyleBase::run(blade);
    RenderColumn(fraction * (NELEM(imageoffsets) - 1));
  }
};


// ContinuousPOV
template<int DEGREES=90>
class ContinousPOV : public POVStyleBase {
  public:
  void run(BladeBase* blade) {
    POVStyleBase::run(blade);
    uint32_t t = micros();
    uint32_t delta = t - last_micros_;
    if (delta > 1000000) delta = 1;
    last_micros_ = t;
    float swing_speed = fusor.swing_speed();
    angle_ = fract(angle_ + swing_speed * delta / 1000000.0 / DEGREES);
    RenderColumn(angle_ * (NELEM(imageoffsets) - 1));
  }

private:
  uint32_t last_micros_ = 0;
  float angle_ = 0.0; // 0..1.0, where 1.0 represents DEGREES degrees.
};

StyleFactoryImpl<Style<StylePOV<>>> style_pov;
#endif

#endif
