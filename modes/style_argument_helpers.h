#ifndef MODE_STYLE_ARGUMENT_HELPERS
#define MODE_STYLE_ARGUMENT_HELPERS

const char* GetStyle(int blade);
void SetStyle(int blade, LSPtr<char> style);

namespace mode {

int menu_current_blade = 1;
int menu_current_arg = 0;

bool isTimeArg(int arg) {
  switch (arg) {
    case IGNITION_TIME_ARG:
    case IGNITION_DELAY_ARG:
    case RETRACTION_TIME_ARG:
    case RETRACTION_DELAY_ARG:
      return true;
    default:
      return false;
  }
}

bool GetArg(int blade, int arg, char* argspace) {
  return style_parser.GetArgument(GetStyle(blade), arg + 2, argspace);
}
void SetArg(int blade, int arg, const char* argument) {
  SetStyle(blade, style_parser.SetArgument(GetStyle(blade), arg + 2, argument));
}

Color16 GetColorArg(int blade, int arg) {
  char argspace[32];
  if (GetArg(blade, arg, argspace)) {
    char* tmp;
    int r = strtol(argspace, &tmp, 0);
    int g = strtol(tmp+1, &tmp, 0);
    int b = strtol(tmp+1, NULL, 0);
    return Color16(r,g,b);
  }
  return Color16(65535,0,0);
}

void SetColorArg(int blade, int arg, Color16 color) {
  char tmp[32];
  itoa(color.r, tmp, 10);
  strcat(tmp, ",");
  itoa(color.g, tmp + strlen(tmp), 10);
  strcat(tmp, ",");
  itoa(color.b, tmp + strlen(tmp), 10);
    
  SetArg(blade, arg, tmp);
}
  
int GetIntArg(int blade, int arg) {
  char argspace[32];
  if (GetArg(blade, arg, argspace)) {
    return strtol(argspace, nullptr, 0);
  }
  return -1;
}
  
void SetIntArg(int blade, int arg, int value) {
  char tmp[32];
  itoa(value, tmp, 10);
  SetArg(blade, arg, tmp);
}

BladeStyle* GetCurrentBladeStyle() {
  BladeBase* blade = GetBladeByNumber(menu_current_blade);
  if (!blade) return nullptr;
  return blade->current_style();
}

int GetMaxStyleArg() {
  BladeStyle* style = GetCurrentBladeStyle();
  if (!style) return ARG_MAX_UNKNOWN;
  return style->get_max_arg(menu_current_arg);
}


}  // namespace mode



#endif
