#ifndef STYLES_STYLE_PARSER_H
#define STYLES_STYLE_PARSER_H

#include "../common/preset.h"
#include "../common/arg_parser.h"
#include "../functions/int_arg.h"

class NamedStyle {
public:
  const char* name;
  StyleAllocator style_allocator;
  const char* description;
};

class BuiltinPresetAllocator : public StyleFactory {
public:
  BladeStyle* make() override {
#if NUM_BLADES == 0
    return nullptr;
#else    
    // Technically we should call run on these.
    IntArg<1, 0> preset_arg;
    IntArg<2, 1> style_arg;
    int preset = preset_arg.getInteger(0);
    int style = style_arg.getInteger(0);
    
    StyleAllocator allocator = nullptr;
    if (preset < 0 || preset >= (int)(current_config->num_presets))
      return nullptr;
    
    Preset* p = current_config->presets + preset;
#define GET_PRESET_STYLE(N) if (style == N) allocator = p->style_allocator##N;
    ONCEPERBLADE(GET_PRESET_STYLE);
    if (!allocator) return nullptr;
    CurrentArgParser->Shift(2);
    // ArgParser ap(SkipWord(CurrentArgParser->GetArg(2, "", "")));
    // CurrentArgParser = &ap;
    return allocator->make();
#endif    
  }
};

BuiltinPresetAllocator builtin_preset_allocator;

NamedStyle named_styles[] = {
#ifndef DISABLE_BASIC_PARSER_STYLES
  { "standard", StyleNormalPtrX<RgbArg<1, CYAN>, RgbArg<2, WHITE>, IntArg<3, 300>, IntArg<4, 800>>(),
    "Standard blade, color, clash color, extension time, retraction time",
  },
  // Combine onspark, inoutsparktip, gradient, customizable blast/clash/lockup colors
  { "advanced",
    StylePtr<
      InOutSparkTipX<
        SimpleClash<
          Lockup<
            Blast<
              OnSparkX<
                Gradient<RgbArg<1, Red>, RgbArg<2, Blue>, RgbArg<3, Green>>,
                RgbArg<4, White>,
                IntArg<5, 10>
              >,
              RgbArg<6, White>, // blast color
              200, 100, 400
            >,
            AudioFlicker<RgbArg<7, Magenta>, White>
          >,
          RgbArg<8, White>,
          40
        >,
        InOutFuncX<IntArg<9, 300>, IntArg<10, 800> >,
        RgbArg<11, White>
      >
    >(),
    "Advanced blade, color at hilt, middle color, tip color, onspark color, onspark time, blast color, lockup color, clash color, extension time, retraction time, spark tip color",
  },
  { "fire",
    StyleFirePtr<RgbArg<1, RED>, RgbArg<2, YELLOW>>(),
    "Fire blade, warm color, hot color"
  },
  { "unstable",
    StylePtr<InOutHelperX<LocalizedClash<Lockup<Blast<OnSpark<BrownNoiseFlicker<Strobe<RgbArg<1, Rgb<150, 0, 0>>,Sparkle<RgbArg<3, Rgb<255,40,0>>, RgbArg<4, Rgb<255,255,10>>,100,1024>,100,50>,Strobe<RgbArg<2, Red>,RgbArg<1, Rgb<150, 0, 0>>,50,5>,100>,White,100>,White,200,100,400>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Yellow,50,1>,Strobe<RgbArg<2, Red>,Black,50,1>,50>,White,200>,White>,AudioFlicker<OnSpark<BrownNoiseFlicker<Strobe<Black,Yellow,50,1>,Strobe<RgbArg<2, Red>,Black,50,1>,50>,White,200>,White>>,White,60,100>,InOutFuncX<IntArg<5, 100>,IntArg<6, 200>>,Black>>(),
    "Unstable blade, warm, warmer, hot, sparks, extension time, retraction time"
  },
  { "strobe",
    StyleNormalPtrX<StrobeX<RgbArg<1, BLACK>, RgbArg<2, WHITE>, IntArg<3, 15>, IntArg<4, 1>>, Rainbow, IntArg<5, 300>, IntArg<6, 800>>(),
    "Stroboscope, standby color, flash color, flash frequency, flash milliseconds, extension time, retraction time"
  },
  { "cycle",
    StylePtr<ColorCycle<RgbArg<1, Blue>,0,1,Layers<
        AudioFlicker<RgbArg<3, Cyan>, RgbArg<2, Blue>>,
        BlastL<RgbArg<4, Rgb<255,50,50>>>,
        LockupL<HumpFlicker<RgbArg<5, Red>, RgbArg<3, Cyan>,100>>,
        SimpleClashL<White>>,
      100,2000,1000>>(),
    "Cycle blade, start color, base color, flicker color, blast color, lockup color"
  },
  { "rainbow", StyleRainbowPtrX<IntArg<1, 300>, IntArg<2, 800>>(),
    "Rainbow blade, extension time, retraction time"
  },
  { "charging", &style_charging, "Charging style" },
#endif
  { "builtin", &builtin_preset_allocator,
    // TODO: Support multiple argument templates.
    "builtin preset styles, "
    "preset number, blade number, "
    "base color, alt color, style option, "
    "ignition option, ignition time, ignition delay, ignition color, ignition power up, "
    "blast color, clash color, lockup color, lockup position, drag color, drag size, lb color, "
    "stab color, melt size, swing color, swing option, emitter color, emitter size, "
    "preon color, preon option, preon size, "
    "retraction option, retraction time, retraction delay, retraction color, retract cooldown, "
    "postoff color, off color, off option, "
    "2nd alt color, 3rd alt color, "
    "2nd style option, 3rd alt option, "
    "ignition bend option, retraction bend option"
  },
};

class StyleParser : public CommandParser {
public:

  NamedStyle* FindStyle(const char *name) {
    if (!name) return nullptr;
    for (size_t i = 0; i < NELEM(named_styles); i++) {
      if (FirstWord(name, named_styles[i].name)) {
	return named_styles + i;
      }
    }

    return nullptr;
  }

  BladeStyle* Parse(const char* str) {
    NamedStyle* style = FindStyle(str);
    if (!style) return nullptr;
    ArgParser ap(SkipWord(str));
    CurrentArgParser = &ap;
    return style->style_allocator->make();
  }

  // Returns true if the listed style refereces the specified argument.
  bool UsesArgument(const char* str, int argument) {
    NamedStyle* style = FindStyle(str);
    if (!style) return false;
    if (argument == 0) return true;
    char unused_output[32];
    GetArgParser ap(SkipWord(str), argument, unused_output);
    CurrentArgParser = &ap;
    delete style->style_allocator->make();
    return ap.next();
  }

  // Returns true if the listed style refereces the specified argument.
  int MaxUsedArgument(const char* str) {
    NamedStyle* style = FindStyle(str);
    if (!style) return false;
    GetMaxArgParser ap(SkipWord(str));
    CurrentArgParser = &ap;
    delete style->style_allocator->make();
    // Ignore the two "builtin" arguments
    if (FirstWord(str, "builtin") && ap.max_arg() <= 2) return 0;
    return ap.max_arg();
  }

  // Get the Nth argument of a style string.
  // The output will be copied to |output|.
  // If the string itself doesn't contain that argument, the style
  // will be parsed, and it's default argument will be returned.
  bool GetArgument(const char* str, int argument, char* output) {
    NamedStyle* style = FindStyle(str);
    if (!style) return false;
    if (argument >= 0) {
      GetArgParser ap(SkipWord(str), argument, output);
      CurrentArgParser = &ap;
      delete style->style_allocator->make();
      if (ap.next()) return true;
    }
    if (argument >= CountWords(str)) {
      *output = 0;
      return false;
    }
    while (argument > 0) {
      str = SkipWord(str);
      argument--;
    }
    str = SkipSpace(str);
    const char* tmp = SkipWord(str);
    memcpy(output, str, tmp - str);
    output[tmp-str] = 0;
    if (!strcmp(output, "~")) {
      *output = 0;
      return false;
    }
    return true;
  }

  // Replace the Nth argument of a style string with a new value and return
  // the new style string. Missing arguments will be replaced with default
  // values.
  LSPtr<char> SetArgument(const char* str, int argument, const char* new_value) {
    char ret[512];  // maximum length for now
    char* tmp = ret;
    int output_args = std::max<int>(CountWords(str), argument + 1);
    for (int i = 0; i < output_args; i++) {
      if (i) strcat(tmp++, " ");
      if (i == argument) {
	strcat(tmp, new_value);
	tmp += strlen(tmp);
      } else {
	if (!GetArgument(str, i, tmp)) {
	  strcat(tmp, "~");
	}
//	fprintf(stderr, "OUTPUT: %s\n", tmp);
	tmp += strlen(tmp);
      }
    }
    *tmp = 0;
    return LSPtr<char>(mkstr(ret));
  }

  // Returns the length of the style identifier.
  // The style identifier might be a single word, or it
  // can be "builtin X Y" where X and Y are numbers.
  static int StyleIdentifierLength(const char* str) {
    const char* end = SkipWord(str);
    if (FirstWord(str, "builtin")) {
      end = SkipWord(SkipWord(end));
    }
    return end - str;
  }

  // Truncates all arguments and just returns the style identifier.
  LSPtr<char> ResetArguments(const char* str) {
    int len = StyleIdentifierLength(str);
    char* ret = (char*) malloc(len + 1);
    if (ret) {
      memcpy(ret, str, len);
      ret[len] = 0;
    }
    return LSPtr<char>(ret);
  }

  // Takes the style identifier "builtin X Y" from |to| and the
  // arguments from |from| and puts them together into one string.
  LSPtr<char> CopyArguments(const char* from, const char* to) {
    int from_style_length = StyleIdentifierLength(from);
    int to_style_length = StyleIdentifierLength(to);
    int len = strlen(from) - from_style_length + to_style_length;
    char* ret = (char*) malloc(len + 1);
    if (ret) {
      memcpy(ret, to, to_style_length);
      ret[to_style_length] = 0;
      strcat(ret, from + from_style_length);
    }
    return LSPtr<char>(ret);
  }

  struct ArgumentHelper {
    const char* str;
    int parts[3];
    ArgumentHelper(const char*str_, int n) : str(str_) {
      parts[0]= StyleIdentifierLength(str);
      const char* tmp = str + parts[0];
      for (int i = 0; i < n; i++) {
	tmp = SkipWord(tmp);
      }
      parts[1] = tmp - str;
      parts[2] = strlen(str);
    }
    int partlen(int part) {
      if (part == 0) return parts[0];
      return parts[part] - parts[part - 1];
    }
    const char* partptr(int part) {
      if (part == 0) return str;
      return str + parts[part - 1];
    }
    void AppendPart(int part, char** to) {
      int l = partlen(part);
      memcpy(*to, partptr(part), l);
      (*to) += l;
      **to = 0;
    }
  };

  // Takes the style identifier "builtin X Y" from |to| and the
  // arguments from |from| and puts them together into one string.
  // Arguments after |keep_arguments_after| are also taken from |to|.
  LSPtr<char> CopyArguments(const char* from, const char* to, int keep_arguments_after) {
    ArgumentHelper from_helper(from, keep_arguments_after);
    ArgumentHelper to_helper(to, keep_arguments_after);
    char* ret = (char*) malloc(from_helper.partlen(0) + to_helper.partlen(1) + from_helper.partlen(2) + 1);
    if (ret) {
      char* tmp = ret;
      to_helper.AppendPart(0, &tmp);
      from_helper.AppendPart(1, &tmp);
      to_helper.AppendPart(2, &tmp);
    }
    return LSPtr<char>(ret);
  }

  struct ArgumentIterator {
    const char* start;
    const char* end;
    ArgumentIterator(const char* str_) : start(str_) {
      end = str_ + StyleIdentifierLength(str_);
    }
    void next() {
      start = end;
      end = SkipWord(end);
    }
    operator bool() const { return end > start; }
    int len() const {
      if (end == start) return 2;
      return end - start;
    }
    void append(char** to) const {
      int l = len();
      memcpy(*to, end == start ? " ~" : start, l);
      (*to) += l;
      **to = 0;
    }
  };

  static bool keep(int arg, const int* arguments_to_keep, size_t arguments_to_keep_len) {
    if (arg == 0) return true;
    for (size_t x = 0; x < arguments_to_keep_len; x++)
      if (arguments_to_keep[x] == arg)
	return true;
    return false;
  }

  // Takes the style identifier "builtin X Y" from |to| and the
  // arguments from |from| and puts them together into one string.
  // Arguments listed in |arguments_to_keep| are also taken from the |from| string.
  LSPtr<char> CopyArguments(const char* from, const char* to, const int* arguments_to_keep, size_t arguments_to_keep_len) {
    int len = 0;
    {
      ArgumentIterator FROM(from);
      ArgumentIterator TO(to);
      for (int arg = 0; FROM || TO; arg++, FROM.next(), TO.next()) {
	if (keep(arg, arguments_to_keep, arguments_to_keep_len)) {
	  len += TO.len();
	} else {
	  len += FROM.len();
	}
      }
    }
    char* ret = (char*) malloc(len + 1);
    if (ret) {
      char* tmp = ret;
      ArgumentIterator FROM(from);
      ArgumentIterator TO(to);
      for (int arg = 0; FROM || TO; arg++, FROM.next(), TO.next()) {
	if (keep(arg, arguments_to_keep, arguments_to_keep_len)) {
	  TO.append(&tmp);
	} else {
	  FROM.append(&tmp);
	}
      }
    }
    // STDOUT << "CopyArguments(from=" << from << " to=" << to << ") = " << ret << "\n";
#if defined(DEBUG)
    if (strlen(ret) != len) {
      STDOUT << "FATAL ERROR IN COPYARGUMENTS: len = " << len << " strlen = " << strlen(ret) << "\n";
    }
#endif    
    return LSPtr<char>(ret);
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "list_named_styles")) {
      // Just print one per line.
      // Skip the last one (builtin)
      for (size_t i = 0; i < NELEM(named_styles) - 1; i++) {
	STDOUT.println(named_styles[i].name);
      }
      for (size_t i = 0; i < current_config->num_presets; i++) {
	for (size_t j = 1; j <= NUM_BLADES; j++) {
	  STDOUT << "builtin " << i << " " << j << "\n";
	}
      }
      return true;
    }

    if (!strcmp("describe_named_style", cmd)) {
      if (NamedStyle* style = FindStyle(arg)) {
	STDOUT.println(style->description);
	ArgParserPrinter arg_parser_printer(SkipWord(arg));
	CurrentArgParser = &arg_parser_printer;
	do {
	  BladeStyle* tmp = style->style_allocator->make();
	  delete tmp;
	} while (arg_parser_printer.next());
      }
      return true;
    }

    return false;
  }
};

StyleParser style_parser;

#endif  // STYLES_STYLE_PARSER_H
