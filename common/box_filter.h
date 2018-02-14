#ifndef COMMON_BOX_FILTER_H
#define COMMON_BOX_FILTER_H

template<class T, int N>
class BoxFilter {
public:
  T filter(const T& v) {
    data[pos] = v;
    pos++;
    if (pos == N) pos = 0;

    T ret = data[0];
    for (int i = 1; i < N; i++) {
      ret += data[i];
    }
    return ret / N;
  }

  T data[N];
  int pos = 0;
};

#endif
