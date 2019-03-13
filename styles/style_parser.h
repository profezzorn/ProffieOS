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
    // Technically we should call run on these.
    IntArg<1> preset_arg;
    IntArg<2> style_arg;
    int preset = preset_arg.getInteger(0);
    int style = style_arg.getInteger(0);
    
    StyleAllocator allocator = nullptr;
    if (preset < 0 && preset >= current_config->num_presets)
      return nullptr;
    
    Preset* p = current_config->presets + preset;
#define GET_PRESET_STYLE(N) if (style == N) allocator = p->style_allocator##N;
    ONCEPERBLADE(GET_PRESET_STYLE);
    if (!allocator) return nullptr;
    return allocator->make();
  }
};

BuiltinPresetAllocator builtin_preset_allocator;

NamedStyle named_styles[] = {
  { "standard", StyleNormalPtrX<RgbArg<1>, RgbArg<2>, IntArg<3>, IntArg<4>>(),
    "Standard blade, color, clash color, extension time, retraction time",
  },
  { "rainbow", StyleRainbowPtrX<IntArg<1>, IntArg<2>>(),
    "Rainbow blade, extension time, retraction time"
  },
  { "builtin", &builtin_preset_allocator,
    "builtin preset styles, preset num, style index"
  },
};

class StyleParser : public CommandParser {
public:

  NamedStyle* FindStyle(const char *name) {
    if (!name) return nullptr;
    for (size_t i = 0; i < NELEM(named_styles); i++)
      if (!strcmp(named_styles[i].name, name))
	return named_styles + i;

    return nullptr;
  }

  BladeStyle* Parse(const char* str) {
    for (size_t i = 0; i < NELEM(named_styles); i++) {
      if (FirstWord(str, named_styles[i].name)) {
	ArgParser ap(SkipWord(str));
	CurrentArgParser = &ap;
	return named_styles[i].style_allocator->make();
      }
    }
    return nullptr;
  }

  bool Parse(const char *cmd, const char* arg) override {
    if (!strcmp(cmd, "list_named_styles")) {
      // Just print one per line.
      for (size_t i = 0; i < NELEM(named_styles); i++) {
	STDOUT.println(named_styles[i].name);
      }
      return true;
    }

    if (!strcmp("describe_named_style", cmd)) {
      if (NamedStyle* style = FindStyle(arg)) {
	STDOUT.println(style->description);
	ArgParserPrinter arg_parser_printer;
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

  void Help() override {
    STDOUT.println(" list_named_styles - List all available named styles");
    STDOUT.println(" describe_named_style <style> - show what arguments a style requires");
  }
};

StyleParser style_parser;

#endif  // STYLES_STYLE_PARSER_H
