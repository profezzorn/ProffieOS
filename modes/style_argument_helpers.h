#ifndef MODE_STYLE_ARGUMENT_HELPERS
#define MODE_STYLE_ARGUMENT_HELPERS

namespace mode {

int menu_current_blade = 1;
int menu_current_arg = 0;
  
Color16 GetColorArg(int blade, int arg) {
  char argspace[32];
  if (style_parser.GetArgument(current_preset_.GetStyle(blade), arg + 2, argspace)) {
    char* tmp;
    int r = strtol(argspace, &tmp, 0);
    int g = strtol(tmp+1, &tmp, 0);
    int b = strtol(tmp+1, NULL, 0);
    return Color16(r,g,b);
  }
  return Color16(65535,0,0);
}

void SetArg(int blade, int arg, const char* argument) {
  current_preset_.SetStyle(blade, style_parser.SetArgument(current_preset_.GetStyle(blade), arg + 2, argument));
}

void SetColorArg(int blade, int arg, Color16 color) {
  char tmp[32];
  itoa(color.r, tmp, 10);
  strcat(tmp, ",");
  itoa(color.g, tmp + strlen(tmp), 10);
  strcat(tmp, ",");
  itoa(color.b, tmp + strlen(tmp), 10);

  SetArg(menu_current_blade, menu_current_arg, tmp);

  current_preset_.Save();
}

}  // namespace mode

#endif
