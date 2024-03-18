#ifndef MODE_STYLE_ARGUMENT_HELPERS
#define MODE_STYLE_ARGUMENT_HELPERS

namespace mode {

int menu_current_blade = 1;
int menu_current_arg = 0;
  
}  // namespace mode

Color16 GetColorArg(int blade, int arg);
void SetArg(int blade, int arg, const char* argument);
void SetColorArg(int blade, int arg, Color16 color);

#endif
