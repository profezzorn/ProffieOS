#if PROFFIEBOARD_VERSION - 0 == 1
#include "proffieboard_v1_config.h"
#elif PROFFIEBOARD_VERSION - 0 == 2
#include "proffieboard_v2_config.h"
#else
#error UNKNOWN PROFFIEBOARD
#endif

