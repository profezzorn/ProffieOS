#ifndef COMMON_PRESET_H
#define COMMON_PRESET_H

#include "malloc_helper.h"

#define CONFIGARRAY(X) X, NELEM(X)

#if NUM_BLADES == 0
#define ONCEPERBLADE(F)
#elif NUM_BLADES == 1
#define ONCEPERBLADE(F) F(1)
#elif NUM_BLADES == 2
#define ONCEPERBLADE(F) F(1) F(2)
#elif NUM_BLADES == 3
#define ONCEPERBLADE(F) F(1) F(2) F(3)
#elif NUM_BLADES == 4
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4)
#elif NUM_BLADES == 5
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5)
#elif NUM_BLADES == 6
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6)
#elif NUM_BLADES == 7
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7)
#elif NUM_BLADES == 8
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8)
#elif NUM_BLADES == 9
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9)
#elif NUM_BLADES == 10
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10)
#elif NUM_BLADES == 11
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11)
#elif NUM_BLADES == 12
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12)
#elif NUM_BLADES == 13
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13)
#elif NUM_BLADES == 14
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13) F(14)
#elif NUM_BLADES == 15
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13) F(14) F(15)
#elif NUM_BLADES == 16
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13) F(14) F(15) F(16)
#elif NUM_BLADES == 17
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13) F(14) F(15) F(16) F(17)
#elif NUM_BLADES == 18
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13) F(14) F(15) F(16) F(17) F(18)
#elif NUM_BLADES == 19
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13) F(14) F(15) F(16) F(17) F(18) F(19)
#elif NUM_BLADES == 20
#define ONCEPERBLADE(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7) F(8) F(9) F(10) F(11) F(12) F(13) F(14) F(15) F(16) F(17) F(18) F(19) F(20)
#else
#error NUM_BLADES is too big
#endif

struct Preset {
  // Sound font.
  const char* font;

  // Sound track
  const char* track;

  // Blade config.
#define DEFINE_BLADE_STYLES(N) StyleAllocator style_allocator##N;
  ONCEPERBLADE(DEFINE_BLADE_STYLES);

  const char* name;
};

#endif
