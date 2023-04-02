#ifndef STYLES_REMAP_H
#define STYLES_REMAP_H

// Remap, remap LED positions
// Usage: Remap<F,COLOR>
// F: FUNCTION - the remapping function
// COLOR: COLOR - color values to remap
// Returns: COLOR
// F is a function which returns values between 0 and 32768
// For each pixel, we'll call F and read the corresponding
// location COLOR, with interpolation.
// So if F is a ramp from 0 to 32768 (0 at base 32768 at tip) then nothing changes.
// If F is 32768 at base and 0 at tip, the blade is reversed.
// Other suitable functions: bump, smoothstep, sin, etc.

template<class F, class COLOR>
class Remap {
 private:
  F f_;
  COLOR color_;
  int num_leds_;

 public:
  LayerRunResult run(BladeBase* blade) {
    f_.run(blade);
    num_leds_ = blade->num_leds();
    return RunLayer(&color_, blade);
  }
  
  auto getColor(int led) -> decltype(MixColors(color_.getColor(led),
					       color_.getColor(led), 1, 15)) {
    int pos = f_.getInteger(led);
    led = clamp(pos * num_leds_, 0, num_leds_ * 32768 - 1);
    int fraction = led & 0x7fff;
    led = clamp(led >> 15, 0, num_leds_);
    return MixColors(color_.getColor(led),
		     color_.getColor(std::min<int>(led + 1, num_leds_ - 1)),
		     fraction,
		     15);
  }
};

#endif  // STYLES_REMAP_H
