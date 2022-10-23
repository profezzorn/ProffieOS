#ifndef COMMON_BOX_FILTER_H
#define COMMON_BOX_FILTER_H

template<class T, int N>
class BoxFilter {
public:
  void clear(const T& v) {
    for (int i = 1; i < N; i++) {
      data[i] = v;
    }
  }
  void push(const T& v) {
    data[pos] = v;
    pos++;
    if (pos == N) pos = 0;
  }
  T get() const {
    T ret = data[0];
    for (int i = 1; i < N; i++) {
      ret += data[i];
    }
    return ret / N;
  }
  T filter(const T& v) {
    push(v);
    return get();
  }

  T data[N];
  int pos = 0;
};

template<class T>
class BoxFilter<T, 1> {
public:
  void push(const T& v) {
    data = v;
  }
  void clear(const T& v) {
    data = v;
  }
  T get() const {
    return data;
  }
  T filter(const T& v) {
    push(v);
    return get();
  }
  T data;
};

template<class T>
class BoxFilter<T, 0> {
public:
  void push(const T& v) {
    data = v;
  }
  void clear(const T& v) {
    data = v;
  }
  T get() const {
    return data;
  }
  T filter(const T& v) {
    push(v);
    return get();
  }
  T data;
};

template<class T>
class BoxFilter<T, 2> {
public:
  void clear(const T& v) {
    data[0] = data[1] = v;
  }
  void push(const T& v) {
    data[0] = data[1];
    data[1] = v;
  }
  T get() const {
    return (data[0] + data[1]) / 2;
  }
  T filter(const T& v) {
    push(v);
    return get();
  }
  T data[2];
};

#endif
