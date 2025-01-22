#ifndef COMMON_PRESET_H
#define COMMON_PRESET_H

#include "malloc_helper.h"
#include "onceperblade.h"

#define CONFIGARRAY(X) X, NELEM(X)


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
