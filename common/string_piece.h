#ifndef COMMON_STRING_PIECE_H
#define COMMON_STRING_PIECE_H

int constexpr toLower(char x) {
  return (x >= 'A' && x <= 'Z') ? x - 'A' + 'a' : x;
}

class Print;
struct StringPiece {
  StringPiece(const char* s) : str(s), len(strlen(s)) {}
  StringPiece(const char* s, size_t l) : str(s), len(l) {}
  StringPiece(const char* begin, const char* end) : str(begin), len(end >= begin ? end - begin : 0) {}
  StringPiece() : str(0), len(0) {}
  int cmp(const StringPiece& other) const {
    const char* a = str;
    const char* b = other.str;
    size_t l = 0;
    while (true) {
      if (l == len && l == other.len) return 0;
      if (l == len) return -1;
      if (l == other.len) return 1;
      char A = toLower(*a);
      char B = toLower(*b);
      if (A != B) return A < B ? -1 : 1;
      a++;
      b++;
      l++;
    }
  }
  bool operator<(const StringPiece& other) const { return cmp(other) < 0; }
  bool operator>(const StringPiece& other) const { return cmp(other) > 0; }
  bool operator<=(const StringPiece& other) const { return cmp(other) <= 0; }
  bool operator>=(const StringPiece& other) const { return cmp(other) <= 0; }
  bool operator==(const StringPiece& other) const { return cmp(other) == 0; }
  bool operator!=(const StringPiece& other) const { return cmp(other) != 0; }
  char operator[](size_t x) const { return str[x]; }
  explicit operator bool() const { return str != nullptr && len != 0; }

  void printTo(Print& p) const ;
  void paste(char* to) const { memcpy(to, str, len); }
  void pasteZ(char* to) const { paste(to); to[len] = 0; }

  bool contains(char c) const {
    for (size_t i = 0; i < len; i++) {
      if (str[i] == c) return true;
    }
    return false;
  }

  const char* str;
  size_t len;
};

#endif
