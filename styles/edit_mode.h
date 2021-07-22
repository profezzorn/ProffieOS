#ifndef STYLES_EDIT_MODE_H
#define STYLES_EDIT_MODE_H

#include "../functions/int_arg.h"
#include "../functions/islessthan.h"
#include "../functions/scale.h"
#include "../functions/wavlen.h"
// Edit Mode Style Arguments / Variables

enum RgbArgumentName {
  BASE_COLOR_ARG = 1, // Primary Base Color
  ALT_COLOR_ARG = 2, // Alternate or Force Color (free-use)
  BLAST_COLOR_ARG = 3, // Blast Color
  CLASH_COLOR_ARG = 4, // Clash Color
  LOCKUP_COLOR_ARG = 5, // Lockup Color
  DRAG_COLOR_ARG = 6, // Drag Color
  LB_COLOR_ARG = 7, // Lightning Block Color
  STAB_COLOR_ARG = 8, // Stab Color
  PREON_COLOR_ARG = 9, // PreOn Color
  IGNITION_COLOR_ARG = 10, // Ignition (Power Up) Color
  RETRACTION_COLOR_ARG = 11, // Retraction (Cool Down) Color
  POSTOFF_COLOR_ARG = 12, // PostOff Color
  SWING_COLOR_ARG = 13, // Swing Color (Responsive Swing Effects)
  EMITTER_COLOR_ARG = 14, // Emitter Color (Emitter Spark) can be used for PostOff
  OFF_COLOR_ARG = 15, // Off Color (Color when blade is Off, for crystals and accents)
};

enum IntArgumentName {
  STYLE_OPTION_ARG = 16, // Style Option
  IGNITION_OPTION_ARG = 17, // Ignition Options
  IGNITION_TIME_ARG = 18, // used in IgnitionTime alias
  RETRACTION_OPTION_ARG = 19, // Retraction Options
  RETRACTION_TIME_ARG = 20, // used in RetractionTime alias
  LOCKUP_POSITION_ARG = 21, // Clash/Lockup Location for Responsive Effects (mid-point)
  DRAG_SIZE_ARG = 22, // Drag Size 
  EMITTER_SIZE_ARG = 23, // Emitter Size (Emitter Spark), can be used for PostOff
  PREON_OPTION_ARG = 24, // PreOn Option
  PREON_SIZE_ARG = 25, // PreOn Size
};

// IgnitionTime = 18 - uses "Auto" feature with WavLen when = 0
template<int DEFAULT_VALUE = 300> using IgnitionTime = Scale<IsLessThan<IntArg<IGNITION_TIME_ARG,DEFAULT_VALUE>,Int<1>>,IntArg<IGNITION_TIME_ARG,DEFAULT_VALUE>,WavLen<EFFECT_IGNITION>>;

// RetractionTime = 20 - uses "Auto" feature with WavLen when = 0
template<int DEFAULT_VALUE = 0> using RetractionTime = Scale<IsLessThan<IntArg<RETRACTION_TIME_ARG,DEFAULT_VALUE>,Int<1>>,IntArg<RETRACTION_TIME_ARG,DEFAULT_VALUE>,WavLen<EFFECT_RETRACTION>>;

#endif
