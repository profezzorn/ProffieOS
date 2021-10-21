#ifndef COMMON_STRFUN_H
#define COMMON_STRFUN_H

int constexpr toLower(char x) {
  return (x >= 'A' && x <= 'Z') ? x - 'A' + 'a' : x;
}

const char *startswith(const char *prefix, const char* x) {
  while (*prefix) {
    if (toLower(*x) != toLower(*prefix)) return nullptr;
    prefix++;
    x++;
  }
  return x;
}

int cmpdir(const char *a, const char *b) {
  while (toLower(*a) == toLower(*b)) {
    if (!*a) return 0;
    a++;
    b++;
  }
  if (*a == '/' && *b == 0) return 0;
  if (*b == '/' && *a == 0) return 0;
  return toLower(*a) - toLower(*b);
}

int parse1hex(const char* x) {
  int ret = toLower(*x);
  if (ret > 'a') return ret - 'a' + 10;
  return ret - '0';
}

int parse2hex(const char* x) {
  return (parse1hex(x) << 4) | parse1hex(x+1);
}

bool endswith(const char *postfix, const char* x) {
  size_t l = strlen(x);
  if (l < strlen(postfix)) return false;
  x = x + l - strlen(postfix);
  while (*postfix) {
    if (toLower(*x) != toLower(*postfix)) return false;
    postfix++;
    x++;
  }
  return true;
}

const char* SkipSpace(const char* str) {
  while (*str == ' ' || *str == '\t') str++;
  return str;
}

const char* SkipWord(const char* str) {
  str = SkipSpace(str);
  while (*str != ' ' && *str != '\t' && *str) str++;
  return str;
}

int CountWords(const char* str) {
  int words = 0;
  while (*str) {
    str = SkipWord(str);
    words++;
  }
  return words;
}

float parsefloat(const char* s) {
  float ret = 0.0;
  float sign = 1.0;
  float mult = 1.0;
  s = SkipSpace(s);
  if (*s == '-') {
    sign = -1.0;
    s++;
  }
  while (*s) {
    int c = *s;
    if (c >= '0' && c <= '9') {
      if (mult == 1.0) {
	ret = (c - '0') + 10 * ret;
      } else {
	ret += (c - '0') * mult;
	mult /= 10.0;
      }
      s++;
    } else if (c == '.') {
      if (mult != 1.0) return ret * sign;
      // Time to read decimals.
      mult /= 10.0;
      s++;
    } else {
      return ret * sign;
    }
  }
  return ret * sign;
}

#endif
