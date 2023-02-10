#ifndef STYLES_POV_H
#define STYLES_POV_H

// TODO(hubbe): Make it work with FastLED
#if defined(ENABLE_WS2811)

/* 
This style draws images in the air if you swing the saber
carefully back and forth. Unfortunately you can't really see
the result without a slow exposure camera. It's possible that
dotstar APA102 pixels would be fast enough to do the effect
full justice, but I haven't tried that.

Usage: StylePOV<>
or: StylePOV<MIN_DEGREES, MAX_DEGREES, REPEAT, MIRROR>
MIN_DEGREES: integer (default -45)
MAX_DEGREES: integer (default 45)
REPEAT: bool (default false)
MIRROR: bool (default true)
return value: COLOR
Since MIN_DEGREES, MAX_DEGREES, REPEAT and MIRROR all have defaults, those arguments can be omitted.
StylePOV<> draws one instance of the image in the top portion of the swing "window".
MIN and MAX degrees set the start and end points for the edges of the image.
REPEAT draws multple instances of the image through the 360 arc, butting each next image adjacent to the previous.
When MIRROR is used with REPEAT, every other image is flipped.  
In older files, you may see &style_pov used, which is a shorhand template, and essentially the same as StylePtr<StylePOV<>>().

Usage:ContinuousPOV<>
or: ContinuousPOV<DEGREES>
DEGREES: integer (default 90)
return value: COLOR
ContinuousPOV<> draws the image multiple times continuously through 360 degrees of the swing, similar to the REPEAT option of StylePOV<>.
However, while StylePOV tries to always display the same line when the saber is at a particular angle,
ContinuousPOV simply moves from line to line, and the speed is controlled by the swing speed.
If you move the saber left and then right, StylePOV will go back and show the previously displayed part of the image,
whereas ContinuousPOV will continue forward through the "scan" on the image data.


Examples as a stand-alone style in a preset:
   { "Font", "tracks/track.wav", &style_pov, "my_pov"}
or { "Font", "tracks/track.wav", StylePtr<StylePOV<>>(), "my_pov"}
or { "Font", "tracks/track.wav", StylePtr<ContinuousPOV<Int<45>>>(), "my_pov"}

Example of use within a blade style:
POV can be used in a layer as the main base blade "color", or as something that shows when swinging,  
or when triggering an effect of your choice, etc... Basic SwingSpeed<> example is shown.  

{ "Font", "tracks/track.wav",  
  StylePtr<Layers<Red,
  AlphaL<ContinuousPOV<>,SwingSpeed<400>>,
  ResponsiveLockupL<White,TrInstant,TrInstant,Int<26000>>,
  InOutTrL<TrWipe<300>,TrWipeIn<500>>>>(),
  "my_continuous_pov"}  
*/


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
class ContinuousPOV : public POVStyleBase {
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
