#ifndef FUNCTIONS_SLICE_H
#define FUNCTIONS_SLICE_H

// Usage: SliceF<DENSITY_FUNCTION>
// or: SliceF<DENSITY_FUNCTION, OFFSET>
// DENSITY_FUNCTION: 3DF 
// OFFSET: integer, defaults to 20
// return value: FUNCTION
// The DENSITY_FUNCTION is a 3-dimensional function, f(x, y, z)
// the SliceF function calculates the x/y/z coordinates based on the
// angle of the blade. For now, the only density functions available
// are SmokeDF and FastSmokeDF, which are basically the same thing.
// This is very similar to how the POV blade works, but instead of
// using a large data blob as input, it just uses another function
// as input.
class SmokeDF {
public:
  float iTime = 0.0;
  void run(BladeBase* blade) {
    iTime = millis() / 1000.0;
  }

  float func(Vec3 uv) {
    uv = Vec3(uv.y, uv.x, uv.z);
    uv.y += iTime / 10.0;
    uv = Vec3(1.0 - uv.y, uv.x, uv.z);
    uv.x += sinf(uv.y * 4.3 + iTime/6.0) / 2.0;
    uv.y += sinf(uv.x * 1.8 - uv.z + iTime/2.0);
    uv.x += sinf(uv.z + uv.y * 3.0 + iTime/3.0);
    uv.y += sinf(uv.x * 2.0  + iTime/2.0);
    return sinf(uv.y*3.0 + uv.x/2.0)/2.0+0.25;
  }
  
  int getInteger(Vec3 pos) {
    return clampi32(func(pos) * 32768, 0, 32768);
  }
};

// Same thing as SmokeDF, but using integer math and
// lookup tables, which makes it a fair amount faster.
class FastSmokeDF {
public:
  int iTime = 0;
  void run(BladeBase* blade) {
    iTime = millis() * 16;
  }

  static int isin(int x) {
#if 1
    return sin_table[(x/100) & 1023];
#else    
    return 16384 * sinf(x / 16384.0);
#endif
  }

  int getInteger(Vec3 uv) {
    int x = 16384 - uv.x * 16384 - iTime / 10;
    int y = uv.y * 16384;
    int z = uv.z * 16384;
    x += isin(y * 43/10 + iTime/6) >> 1;
    y += isin(x * 18/10 - z + iTime/2);
    x += isin(z + y * 3 + iTime/3);
    y += isin(x * 2  + iTime/2);
    return clampi32(isin(y*3 + x/2) + 8192, 0, 32768);
  }
};

template<class DF, int offset=20>
class SliceF {
public:
  void run(BladeBase* blade) {
    angle_ = fusor.down() * (1.0 / blade->num_leds());
    df_.run(blade);
//    STDOUT << "DOWN=" << fusor.down() << "\n";
  }

  int getInteger(int led) {
    // TODK: Add actual location
    return df_.getInteger(angle_ * (led + offset));
  }
private:
  Vec3 angle_;
  PONUA DF df_;
};

#endif
