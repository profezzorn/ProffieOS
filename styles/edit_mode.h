#ifndef STYLES_EDIT_MODE_H
#define STYLES_EDIT_MODE_H

#include "../functions/int_arg.h"
#include "../functions/islessthan.h"
#include "../functions/scale.h"
#include "../functions/wavlen.h"
// Edit Mode Style Arguments / Variables

enum ArgumentName {
  BASE_COLOR_ARG = 1, // Primary Base Color
  ALT_COLOR_ARG = 2, // Alternate or Force Color (free-use)
  STYLE_OPTION_ARG = 3, // Style Option
  IGNITION_OPTION_ARG = 4, // Ignition Options
  IGNITION_TIME_ARG = 5, // used in IgnitionTime alias
  IGNITION_DELAY_ARG = 6, // Ignition Delay Time
  IGNITION_COLOR_ARG = 7, // Ignition "Power Up" Color
  IGNITION_POWER_UP_ARG = 8, // Ignition "Power Up" Options
  BLAST_COLOR_ARG = 9, // Blast Color
  CLASH_COLOR_ARG = 10, // Clash Color
  LOCKUP_COLOR_ARG = 11, // Lockup Color
  LOCKUP_POSITION_ARG = 12, // Clash/Lockup Location for Responsive Effects (mid-point)
  DRAG_COLOR_ARG = 13, // Drag Color
  DRAG_SIZE_ARG = 14, // Drag Size
  LB_COLOR_ARG = 15, // Lightning Block Color
  STAB_COLOR_ARG = 16, // Stab / Melt Color
  MELT_SIZE_ARG = 17, // Stab / Melt Size
  SWING_COLOR_ARG = 18, // Swing Color (Responsive Swing Effects)
  SWING_OPTION_ARG = 19, // Swing Options (Responsive Swing Effects) 
  EMITTER_COLOR_ARG = 20, // Emitter Color (Emitter Spark) can be used for PostOff 
  EMITTER_SIZE_ARG = 21, // Emitter Size (Emitter Spark), can be used for PostOff 
  PREON_COLOR_ARG = 22, // PreOn Color
  PREON_OPTION_ARG = 23, // PreOn Option
  PREON_SIZE_ARG = 24, // PreOn Size 
  RETRACTION_OPTION_ARG = 25, // Retraction Options
  RETRACTION_TIME_ARG = 26, // used in RetractionTime alias
  RETRACTION_DELAY_ARG = 27, // Retraction Delay Time
  RETRACTION_COLOR_ARG = 28, // Retraction "Cool Down" Color
  RETRACTION_COOL_DOWN_ARG = 29, // Retraction "Cool Down" Options
  POSTOFF_COLOR_ARG = 30, // PostOff Color  
  OFF_COLOR_ARG = 31, // Off Color (Color when blade is Off for crystals and accents)
  OFF_OPTION_ARG = 32, // Off Options (when blade is Off, for crystals and accents)
  ALT_COLOR2_ARG = 33, // Generic 2nd Alt Color
  ALT_COLOR3_ARG = 34, // Generic 3rd Alt Color
  STYLE_OPTION2_ARG = 35, // Generic 2nd Style Option
  STYLE_OPTION3_ARG = 36, // Generic 3rd Style Option
  IGNITION_OPTION2_ARG = 37, // Ignition BEND Option
  RETRACTION_OPTION2_ARG = 38, // Retraction BEND Option
  LAST_ARG = RETRACTION_OPTION2_ARG, // End of Argument List
};

// IgnitionTime = IGNITION_TIME_ARG - uses "Auto" feature with WavLen when = 0
template<int DEFAULT_VALUE = 300> using IgnitionTime = Scale<IsLessThan<IntArg<IGNITION_TIME_ARG,DEFAULT_VALUE>,Int<1>>,IntArg<IGNITION_TIME_ARG,DEFAULT_VALUE>,WavLen<EFFECT_IGNITION>>;

// RetractionTime = RETRACTION_TIME_ARG - uses "Auto" feature with WavLen when = 0
template<int DEFAULT_VALUE = 0> using RetractionTime = Scale<IsLessThan<IntArg<RETRACTION_TIME_ARG,DEFAULT_VALUE>,Int<1>>,IntArg<RETRACTION_TIME_ARG,DEFAULT_VALUE>,WavLen<EFFECT_RETRACTION>>;

#endif
