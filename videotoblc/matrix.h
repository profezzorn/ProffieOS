#ifndef MATRIX_H
#define MATRIX_H

#include <assert.h>
#include <math.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <vector>
#include <stdlib.h>

#include "thread_helper.h"

// #define SPARSE_MATRIX

template<class T> static inline bool very_close(T a, T b) { return a == b; }
template<> inline bool very_close(double a, double b) { return fabs(a-b) < 0.0000000000001; }
template<> inline bool very_close(float a, float b) { return fabs(a-b) < 0.000001; }
template<class T> T clamp(T v, T low, T high) {
  if(v < low) return low;
  if(v > high) return high;
  return v;
}

template<class T> inline T sqr(T x) { return x * x; }

extern "C" {
int nnls_c(double* a, const int* mda, const int* m, const int* n, double* b, 
	   double* x, double* rnorm, double* w, double* zz, int* index, 
	   int* mode);
float *nnls(float *a_matrix, float *b_matrix, int height, int width);
};

#ifdef SPARSE_MATRIX

#include <map>

template<class T> class Row {
public:
  typedef typename std::map<int, T>::iterator iterator;
  typedef typename std::map<int, T>::const_iterator const_iterator;
  const_iterator begin() const { return data_.begin(); }
  const_iterator end() const { return data_.end(); }
  iterator begin() { return data_.begin(); }
  iterator end() { return data_.end(); }

  T get(int x) const {
    const_iterator i = data_.find(x);
    if (i == data_.end()) return 0.0;
    return i->second;
  }

  void set(int x, T val) {
    if (very_close<T>(val, 0)) {
      data_.erase(x);
    } else {
      data_[x] = val;
    }
  }

  void add(int x, T val) {
    set(x, get(x) + val);
  }

  void operator*=(T v) {
    for (iterator i = data_.begin(); i != data_.end(); ++i) {
      i->second *= v;
    }
  }

  void operator+=(T v) {
    for (iterator i = data_.begin(); i != data_.end(); ++i) {
      i->second += v;
    }
  }

  bool operator==(const Row& other) const {
    if (data_.size() != other.data_.size()) {
      return false;
    }
    const_iterator a = begin();
    const_iterator b = other.begin();
    while (a != end()) {
      if (b == other.end()) return false;
      if (a->first != b->first) return false;
      if (!very_close(a->second, b->second)) return false;
      ++a;
      ++b;
    }
    return b == other.end();
  }

  void muladd(const Row& other, T v) {
    for (const_iterator i = other.begin(); i != other.end(); ++i) {
      set(i->first, get(i->first) + i->second * v);
    }
  }

  void operator+=(const Row& other) {
    for (const_iterator i = other.begin(); i != other.end(); ++i) {
      set(i->first, get(i->first) + i->second);
    }
  }


  void operator-=(const Row& other) {
    for (const_iterator i = other.begin(); i != other.end(); ++i) {
      set(i->first, get(i->first) - i->second);
    }
  }


  void zero() {
    typename std::map<int, T> tmp;
    tmp.swap(data_);
  }

  void swap(Row &other) {
    data_.swap(other.data_);
  }

  size_t memsize() const {
    return data_.size() * sizeof(typename std::pair<int, T>);
  }

  int largest_used_column() const {
    if (data_.empty()) return 0;
    return data_.rbegin()->first;
  }

  int smallest_used_column() const {
    if (data_.empty()) return 0;
    return data_.begin()->first;
  }

  int width() const {
    if (data_.empty()) return 0;
    return data_.rbegin()->first - data_.begin()->first;
  }

  void normalize() {
    T sum = 0;
    for (iterator i = data_.begin(); i != data_.end(); ++i) {
      sum += i->second;
    }
    sum = 1 / sum;
    for (iterator i = data_.begin(); i != data_.end(); ++i) {
      i->second *= sum;
    }
  }

  T norm(T x, int n) const {
    for (const_iterator i = data_.begin(); i != data_.end(); ++i) {
      if (n > 10000) {
	x = std::max(x, i->second);
      } else {
	x += pow(i->second, 1.0*n);
      }
    }
    return x;
  }

private:
  std::map<int, T> data_;
};

#endif  // SPARSE_MATRIX

template<class T> class Matrix {
 private:
  int *refs;

#ifdef SPARSE_MATRIX
 public:
  typedef Row<T> RowType;

 private:
  int columns_;
  std::vector<Row<T> > *data_;

  void Modify() {
    if (refs && *refs > 1) {
      (*refs)--;
      refs = new int(1);
      data_ = new std::vector<Row<T> >(*data_);
    }
  }

 public:
  const Row<T> &row(int x) const {
    return data_->at(x);
  }

  Row<T> &row(int x) {
    Modify();
    return data_->at(x);
  }

  int rows() const {
    if (!data_) return 0;
    return data_->size();
  }

  int columns() const {
    return columns_;
  }

  Matrix() :
    columns_(0),
    refs(NULL),
    data_(NULL) {
  }

  // Zero matrix
  Matrix(int xsize, int ysize) :
      refs(new int(1)),
      columns_(xsize),
      data_(new std::vector<Row<T> >(ysize)) {
    if (rows()) assert(columns_ > 0);
  }

  // square unity matrix
  explicit Matrix(int size) : 
      refs(new int(1)),
      columns_(size),
      data_(new std::vector<Row<T> >(size)) {
    if (rows()) assert(columns_ > 0);
    unity(1);
  }

  Matrix(const Matrix &x) :
    refs(x.refs),
    columns_(x.columns_),
    data_(x.data_) {
    if(refs) (*refs)++;
    if (rows()) assert(columns_ > 0);
  }

  void free() {
    if (refs) {
      if (0 == --(*refs)) {
	delete data_;
	delete refs;
	data_ = NULL;
	refs = NULL;
      }
    }
  }

  // assignment
  Matrix& operator=(const Matrix &x) {
    if (&x != this) {
      free();
      columns_ = x.columns_;
      data_ = x.data_;
      if (rows()) assert(columns_ > 0);
      refs = x.refs;
      if(refs) (*refs)++;
    }
    return *this;
  }

  template<class X>
  Matrix& copyfrom(const Matrix<X> &x) {
    free();
    columns_ = x.columns();
    data_ = new std::vector<Row<T> >(x.rows());
    for (int y = 0; y < rows(); y++) {
      for (typename Row<X>::const_iterator i = x.row(y).begin();
	   i != x.row(y).end();
	   ++i) {
	set(i->first, y, i->second);
      }
    }
  }

  // Set diagonal to 'x'
  Matrix& operator=(const T &x) {
    Modify();
    unity(x);
    return *this;
  }

  ~Matrix() { free(); }

  void zero() {
    Modify();
    if (!data_) return;
    for (int e=0; e < data_->size(); e++) row(e).zero();
  }

  void unity(T val) {
    Modify();
    if (!data_) return;
    assert(data_->size() == columns_);
    zero();
    for(int e=0; e < columns_; e++) set(e, e, val); 
  }

  const T get(int x, int y) const {
    assert(x >= 0);
    assert(x < columns_);
    return row(y).get(x);
  }

  void set(int x, int y, T v) {
    Modify();
    assert(x >= 0);
    assert(x < columns_);
    return row(y).set(x, v);
  }

  void add(int x, int y, T v) {
    Modify();
    assert(x >= 0);
    assert(x < columns_);
    row(y).add(x, v);
  }

  void addclamped(int x, int y, T v) {
    Modify();
    row(clamp(y, 0, rows() - 1)).add(clamp(x, 0, columns() - 1), v);
  }

  const T clampedvalue(int x, int y) const {
    return get(clamp(x, 0, columns() - 1), clamp(y, 0, rows() - 1));
  }

  size_t memsize() const {
    size_t ret = 0;
    for (int y = 0; y < rows(); y++) {
      ret += row(y).memsize();
    }
    return ret;
  }

  bool operator==(const Matrix &m) const {
    if(m.data_ == data_) return true;
    if(rows() != m.rows()) return false;
    if(columns() != m.columns()) return false;

    for (int y = 0; y < rows(); y++) {
      if (!(row(y) == m.row(y)))
	return false;
    }

    return true;
  }

  Matrix copy() const {
    return *this;
  }

  Matrix operator*(T v) const {
    Matrix ret(columns(), rows());
    for (int y = 0; y < rows(); y++)
      for (typename Row<T>::const_iterator x = row(y).begin(); x != row(y).end(); ++x)
	ret.set(x->first, y, x->second * v);
    return ret;
  }

  template<class X>
  Matrix<X> operator*(const std::vector<X> &v) const {
    assert(columns() == v.size());
    Matrix<X> ret(1, rows());
    for(int y = 0; y < rows(); y++) {
      X tmp(0);
      for (typename Row<T>::const_iterator z = row(y).begin(); z != row(y).end(); ++z) {
	tmp += v[z->first] * z->second;
      }
      ret.set(0, y, tmp);
    }
    return ret;
  }

  template<class X>
  Matrix<X> operator*(const Matrix<X> &m) const {
    assert(columns() == m.rows());
    if (m.columns() == 1) {
      std::vector<X> lookup(m.rows());
      for (int i = 0; i < m.rows(); i++) lookup[i] = m.get(0, i);
      return (*this) * lookup;
    } else {
      Matrix<X> ret(m.columns(), rows());
      for(int y = 0; y < rows(); y++) {
	for (typename Row<T>::const_iterator z = row(y).begin(); z != row(y).end(); ++z) {
	  for (typename Row<X>::const_iterator x = m.row(z->first).begin(); x != m.row(z->first).end(); ++x) {
	    ret.add(x->first, y, x->second * z->second);
	  }
	}
      }
      return ret;
    }
  }
  
  void operator+=(const Matrix& other) {
    Modify();
    assert(columns() == other.columns());
    assert(rows() == other.rows());
    for (int y = 0; y < rows(); y++)
      data_->at(y) += other.row(y);
  }

  void operator-=(const Matrix& other) {
    Modify();
    assert(columns() == other.columns());
    assert(rows() == other.rows());
    for (int y = 0; y < rows(); y++)
      data_->at(y) -= other.row(y);
  }

  void operator+=(T v) {
    Modify();
    for (int y = 0; y < rows(); y++) data_->at(y) += v;
  }

  Matrix operator-(const Matrix& other) {
    Matrix ret(*this);
    ret -= other;
    return ret;
  }

  void addmultiplied(const Matrix &other, T v) {
    Modify();
    assert(columns() == other.columns());
    assert(rows() == other.rows());
    for (int y = 0; y < rows(); y++)
      data_->at(y).muladd(other.row(y), v);
  }

  void normalizerows() {
    Modify();
    for (int y = 0; y < rows(); y++) row(y).normalize();
  }

  void swaprows(int a, int b) {
    Modify();
    row(a).swap(row(b));
  }

  void rowmuladd(int a, int b, T val) {
    Modify();
    row(a).muladd(row(b), val);
  }

  void Reshape(int xsize, int ysize) {
    assert(xsize * ysize == rows() * columns());
    Matrix tmp(xsize, ysize);
    for (int y = 0; y < rows(); y++) {
      for (typename Row<T>::const_iterator x = row(y).begin(); x != row(y).end(); ++x) {
	int p = y * columns() + x->first;
	int row = p % xsize;
	int col = p / xsize;
	tmp.set(row, col, x->second);
      }
    }
    *this = tmp;
  }

  void Print() const {
    for (int y = 0; y < rows(); y++) {
      for (int x = 0; x < columns(); x++) {
	printf("%+7.2f ", get(x, y));
      }
      printf("\n");
    }
    printf("------------------------------\n");
  }

  void PrintEq(const Matrix &other) const {
    for (int y = 0; y < rows(); y++) {
      for (int x = 0; x < columns(); x++) {
	printf("%+7.2f ", get(x, y));
      }
      printf(" | ");
      for (int x = 0; x < other.columns(); x++) {
	printf("%+7.2f ", other.get(x, y));
      }
      printf("\n");
    }
    printf("------------------------------\n");
  }

  // FIXME
  Matrix transpose() const {
    Matrix ret(rows(), columns());
    for (int y = 0; y < rows(); y++) {
      for (typename Row<T>::const_iterator x = row(y).begin(); x != row(y).end(); ++x) {
	ret.set(y, x->first, x->second);
      }
    }

    return ret;
  }

  Matrix left_invert() const {
#if 0
    Matrix t = transpose();
    Matrix aat = (*this) * t;
    return t * aat.invert();
#endif
#if 0
    return solve_least_square(Matrix(rows()));
#endif
#if 0
    fprintf(stderr, "transpose...\n");
    Matrix tmp1 = this->transpose();
    fprintf(stderr, "mult1...\n");
    Matrix tmp2 = tmp1 * *this;
    fprintf(stderr, "solve...\n");
//    tmp2.solve(tmp1);
    Matrix answers(tmp2.rows());
    tmp2.solve(answers);
    fprintf(stderr, "solved...\n");
    return answers * tmp1;
#endif
#if 1
    fprintf(stderr, "transpose...\n");
    Matrix tmp1 = this->transpose();
    fprintf(stderr, "mult1...\n");
    Matrix tmp2 = tmp1 * *this;
    fprintf(stderr, "solve...\n");
    tmp2.solve(tmp1);
    return tmp1;
#endif
  }

  T norm(int n) const {
    T ret = 0.0;
    for (int y = 0; y < rows(); y++) {
      ret = row(y).norm(ret, n);
    }
    if (n < 10000) {
      ret = pow(ret, 1.0/n);
    }
    return ret;
  }

  template<class X>
  bool solve(Matrix<X> &answers) {
    Modify();
    answers.Modify();
    assert(rows() == columns());
    assert(rows() == answers.rows());
    for(int x=0; x < columns(); x++) {
      // fprintf(stderr, "COL: %d\n", x);
      // PrintEq(answers);
      int best_row = -1;
      T best_val = 0;
      int best_row_length = columns();
      for (int y = x; y < rows(); y++) {
#if 0
	if (fabs(get(x, y)) > 0.0001 && row(y).largest_used_column() < best_row_length) {
	  best_val = get(x, y);
	  best_row = y;
	  best_row_length = row(y).largest_used_column();
	}
#endif
#if 1
	if (fabs(get(x, y)) > fabs(best_val)) {
	  best_val = get(x, y);
	  best_row = y;
	}
#endif
      }
      if (best_row == -1) {
	std::cout << "FAIL: x=" << x << "/" << columns() << "\n"<< *this;
      }
      assert(best_row >= 0);
      if (best_row != x) {
	answers.swaprows(x, best_row);
	swaprows(x, best_row);
      }
      if (best_val != 1.0) {
	row(x) *= 1.0 / best_val;
	answers.row(x) *= 1.0 / best_val;
      }
      // PrintEq(answers);
      for(int y = 0; y < rows(); y++) {
	if (x == y) continue;
	if (get(x, y) == 0.0) continue;
	T tmp = - get(x, y);
	answers.rowmuladd(y, x, tmp);
	rowmuladd(y, x, tmp);
      }
    }
#if 1
    for(int x = columns() - 1; x >= 0; x--) {
      for(int y = x - 1; y >= 0; y--) {
	if (get(x, y) == 0.0) continue;
	T tmp = - get(x, y);
	answers.rowmuladd(y, x, tmp);
	rowmuladd(y, x, tmp);
      }
    }
#endif

    return true;
  }

#else // SPARSE_MATRIX
  T *data;

 public:
  int xsize, ysize;

  Matrix() : 
    xsize(0),
    ysize(0),
    data(0),
    refs(0) {
  }

  Matrix(int w, int h) :
    xsize(w),
    ysize(h),
    data(new T[w * h]),
    refs(new int(1)) {
    zero();
  }

  // square unity matrix
  explicit Matrix(int size) : 
    xsize(size),
    ysize(size),
    data(new T[size * size]),
    refs(new int(1)) {
      assert(false);
      unity(1);
  }

  Matrix(const Matrix &x) :
    xsize(x.xsize),
    ysize(x.ysize),
    data(x.data),
    refs(x.refs) {
    if(refs) (*refs)++;
  }

  void free() {
    if (refs) {
      if (0 == --(*refs)) {
	delete[] data;
	delete refs;
	data = 0;
	refs = 0;
      }
    }
  }

  void Reshape(int x, int y) {
    assert(x * y == xsize * ysize);
    xsize = x;
    ysize = y;
  }

  int rows() const {
    return ysize;
  }

  int columns() const {
    return xsize;
  }

  Matrix& operator=(const Matrix &x) {
    if(&x != this) {
      free();
      xsize = x.xsize;
      ysize = x.ysize;
      data = x.data;
      refs = x.refs;
      if(refs) (*refs)++;
    }
    return *this;
  }

  void Modify() {
    if (refs && *refs > 1) {
      (*refs)--;
      refs = new int(1);
      T *x = new T[xsize * ysize];
      for(int e=0;e<xsize*ysize;e++) x[e] = data[e];
      data = x;
    }
  }

  // Set diagonal to 'x'
  Matrix& operator=(const T &x) {
    Modify();
//    for(int e=0;e<xsize*ysize;e++) data[e]=x;
    unity(x);
    return *this;
  }

  ~Matrix() { free(); }

  void zero() {
    Modify();
    for(int e=0;e<xsize*ysize;e++) data[e]=0;
  }

  void unity(T val) {
    Modify();
    assert(xsize == ysize);
    zero();
    for(int e=0;e<xsize;e++)
      data[e * xsize + e]=val;
  }

  void operator+=(T val) {
    Modify();
    for(int e=0;e<xsize*ysize;e++)
      data[e] += val;
  }

  Matrix copy() const {
    Matrix ret(xsize, ysize);
    memcpy(ret.data, data, xsize * ysize * sizeof(T));
    return ret;
  }

  T min() const {
    if (xsize == 0 || ysize == 0) return 0.0;
    T ret = data[0];
    for(int e=1;e<xsize*ysize;e++) if (data[e] < ret) ret = data[e];
    return ret;
  }
  T max() const {
    if (xsize == 0 || ysize == 0) return 0.0;
    T ret = data[0];
    for(int e=1;e<xsize*ysize;e++) if (data[e] > ret) ret = data[e];
    return ret;
  }

  Matrix operator*(T val) {
    Matrix ret(xsize, ysize);
    for(int e=0;e<xsize*ysize;e++)
      ret.data[e] += data[e] * val;
    return ret;
  }

  void operator+=(const Matrix<T> &mat) {
    Modify();
    assert(mat.xsize == xsize);
    assert(mat.ysize == ysize);
    for(int e=0;e<xsize*ysize;e++)
      data[e] += mat.data[e];
  }

  Matrix operator-(const Matrix<T> &mat) const {
    assert(mat.xsize == xsize);
    assert(mat.ysize == ysize);
    Matrix ret(xsize, ysize);
    for(int e=0;e<xsize*ysize;e++)
      ret.data[e] = data[e] - mat.data[e];
    return ret;
  }

  T *row(int y) {
    Modify();
    assert(y>=0);
    assert(y<ysize);
    return data + y * xsize;
  }

  const T &value(int x, int y) const {
    assert(x>=0);
    assert(y>=0);
    assert(x<xsize);
    assert(y<ysize);
    return data[y * xsize + x];
  }

  const T &get(int x, int y) const {
    assert(x>=0);
    assert(y>=0);
    assert(x<xsize);
    assert(y<ysize);
    return data[y * xsize + x];
  }

  T &value(int x, int y) {
    Modify();
    assert(x>=0);
    assert(y>=0);
    assert(x<xsize);
    assert(y<ysize);
    return data[y * xsize + x];
  }

  void set(int x, int y, T v) {
    Modify();
    assert(x>=0);
    assert(y>=0);
    assert(x<xsize);
    assert(y<ysize);
    data[y * xsize + x] = v;
  }

  void swaprows(int a, int b) {
    Modify();
    assert(a!=b);
    assert(a>=0);
    assert(b>=0);
    assert(a<ysize);
    assert(b<ysize);
    T *ap = data + a * xsize;
    T *bp = data + b * xsize;
    for(int e=0;e<xsize;e++) {
      T tmp = *ap;
      *ap = *bp;
      *bp = tmp;
      ap++;
      bp++;
    }
  }

  void rowmulsub(int start, int a, int b, T val) {
    Modify();
    assert(a!=b);
    assert(a>=0);
    assert(b>=0);
    assert(a<ysize);
    assert(b<ysize);
    T *ap = data + a * xsize + start;
    T *bp = data + b * xsize + start; 
    for(int e=start;e<xsize;e++) *(ap++) -= *(bp++) * val;
  }

  void mulrow(int a, T val) {
    Modify();
    assert(a>=0);
    assert(a<ysize);
    T *ap = data + a * xsize;
    for(int e=0;e<xsize;e++) *(ap++) *= val;
  }

  bool solve(Matrix<T> &answers) {
    Modify();
    answers.Modify();
    assert(xsize == ysize);
    assert(ysize == answers.ysize);
    for(int x=0;x<xsize;x++)
    {
      for(int y=x + 1;y<ysize;y++)
      {
	if(value(x,y) == 0.0) continue;
	if(value(x,x) == 0.0) {
	  answers.swaprows(x,y);
	  swaprows(x,y);
	} else {
	  T tmp = value(x,y)/value(x,x);
	  answers.rowmulsub(0, y, x, tmp);
	  rowmulsub(x, y, x, tmp);
	  value(x,y) = 0; 
	}
      }
    }
    return SolveUpper(answers);
  }

  Matrix transpose() const {
    Matrix ret(ysize, xsize);
    for(int y=0;y<ysize;y++)
      for(int x=0;x<xsize;x++)
	ret.value(y,x) = value(x,y);

    return ret;
  }

  Matrix operator*(const Matrix &m) const {
    assert(xsize == m.ysize);
    Matrix ret(m.xsize, ysize);
    for(int z=0;z<xsize;z++) {
      for(int y=0;y<ysize;y++) {
	if(value(z,y) != 0.0) {
	  for(int x=0;x<m.xsize;x++) {
	    ret.value(x,y) += value(z,y) * m.value(x,z);
	  }
	}
      }
    }
    return ret;
  }

  bool operator==(const Matrix &m) const {
    if(m.data == data) return true;
    if(m.xsize != xsize || m.ysize != ysize)
      return false;
    for(int y=0;y<ysize;y++)
      for(int x=0;x<xsize;x++)
	if( ! very_close<T>(m.value(x,y), value(x,y)))
	  return false;
    return true;
  }

  double zp() {
    Modify();
    int zc = 0;
    for(int y=0;y<ysize;y++)
      for(int x=0;x<xsize;x++)
	if(value(x,y) == 0.0)
	  zc++;
    return zc/(double)(xsize * ysize);
  }

  void MinZero() {
    Modify();
    for(int e=0;e<xsize*ysize;e++)
      if (data[e] < 0)
	data[e] = 0;
  }

  T norm(int n) const {
    if (n > 10000) {
      T ret = 0.0;
      for(int e=0;e<xsize*ysize;e++)
	ret = std::max<T>(fabs(data[e]), ret);
      return ret;
    }
    
    T ret = 0.0;
    for(int e=0;e<xsize*ysize;e++)
      ret += pow(data[e], 1.0*n);
    return pow(ret, 1.0/n);
  }

#endif  // SPARSE_MATRIX

  bool SolveLower(Matrix<T> &answers) const {
    assert(rows() == columns());
    assert(rows() == answers.rows());
    for (int x = 0; x < columns(); x++) {
      for (int y = 0; y < x; y++) {
        assert(get(x, y) == 0.0);
      }
    }
    for (int x = 0; x < answers.columns(); x++) {
      for (int y = 0; y < answers.rows(); y++) {
	T tmp = answers.get(x, y);
	for (int i = 0; i < y; i++) {
	  tmp -= get(i, y) * answers.get(x, i);
	}
	if (get(y, y) == 0.0) return false;
	answers.set(x, y, tmp / get(y, y));
      }
    }
    return true;
  }

  bool SolveUpper(Matrix<T> &answers) const {
    assert(rows() == columns());
    assert(rows() == answers.rows());
    for (int x = 0; x < columns(); x++) {
      for (int y = x + 1; y < rows(); y++) {
        assert(get(x, y) == 0.0);
      }
    }
    for (int x = 0; x < answers.columns(); x++) {
      for (int y = answers.rows() - 1; y >= 0; y--) {
	T tmp = answers.get(x, y);
	for (int i = y + 1; i < answers.rows(); i++) {
	  tmp -= get(i, y) * answers.get(x, i);
	}
	if (get(y, y) == 0.0) return false;
	answers.set(x, y, tmp / get(y, y));
      }
    }
    return true;
  }

  Matrix<float> fnnls_low(Matrix Xty) {
    // XtX = this
    // m = xsize
    // n = ysize
    int xsize = columns();
    int ysize = rows();
    double tol = pow(2.0, -52.0) * norm(100000) * std::max<int>(xsize, ysize);
    std::vector<bool> P(ysize, false);
    Matrix<float> x(1, ysize);
    Matrix<float> w = Xty - (*this) * x;
    Matrix<float> z;
    int iter = 0;
    while (true) {
      int t = -1;
      double wt = 0.0;
      for (int i = 0; i < ysize; i++) {
	if (w.get(0, i) > wt && !P[i]) {
	  wt = w.get(0, i);
	  t = i;
	}
      }
      if (wt <= tol) break;
      P[t] = true;
      z = solve(Xty, P);
      while (iter < 30 * ysize) {
	iter++;
	bool got_value = false;
	double alpha;
	for (int i = 0; i < ysize; i++) {
	  if (z.get(0, i) <= tol && P[i]) {
	    double a = x.get(0,i) / (x.get(0,i) - z.get(0,i));
	    if (!got_value) {
	      alpha = a;
	      got_value = true;
	    } else {
	      alpha = std::min(alpha, a);
	    }
	  }
	}
	if (!got_value) break;
	x += (z - x) * alpha;
	for (int i = 0; i < ysize; i++) {
	  if (fabs(x.get(0, i)) < tol) {
	    P[i] = false;
	  }
	}
	z = solve(Xty, P);
      }
      x = z;
      w = Xty - (*this) * x;
    }
    return x;
  }

  Matrix fnnls(Matrix y) const {
    Matrix Xt =  this->transpose();
    Matrix XtX = Xt * *this;
    Matrix Xty = Xt * y;
    return XtX.fnnls_low(Xty);
  }

  Matrix solve_least_square(const Matrix<T> &answers) const {
//    fprintf(stderr, "start\n");
    assert(answers.rows() == rows());
    Matrix tmp1 = this->transpose();
//    fprintf(stderr, "transpose\n");
    Matrix tmp2 = tmp1 * *this;
//    fprintf(stderr, "mult1\n");
    Matrix tmp3 = tmp1 * answers;
//    fprintf(stderr, "mult2\n");
    tmp2.solve(tmp3);
//    fprintf(stderr, "done\n");
    return tmp3;
  }

#if 1
  Matrix invert() const {
    fprintf(stderr, "start\n");
    Matrix tmp1 = this->transpose();
    fprintf(stderr, "transpose\n");
    Matrix tmp2 = tmp1 * *this;
    fprintf(stderr, "mult1\n");
    tmp2.solve(tmp1);
    fprintf(stderr, "done\n");
    return tmp1;
  }
#else
  Matrix invert() const {
    Matrix ret(rows());
    copy().solve(ret);
    return ret;
  }
#endif

  // Julienne, then solve.
  Matrix<T> solve(const Matrix<T> &answers, const std::vector<bool> &P) const {
    std::vector<int> index;
    for (int i = 0; i < P.size(); i++) {
      if (P[i]) {
	index.push_back(i);
      }
    }
    Matrix<T> a(index.size(), index.size());
    Matrix<T> b(1, index.size());
    for (int y = 0; y < index.size(); y++) {
      for (int x = 0; x < index.size(); x++) {
	a.set(x, y, get(index[x], index[y]));
      }
      b.set(0, y, answers.get(0, index[y]));
    }
    a.solve(b);
    Matrix<T> ret(1, P.size());
    for (int y = 0; y < index.size(); y++) {
      ret.set(0, index[y], b.get(0, y));
    }
    return ret;
  }

  Matrix cholesky() const {
    assert(rows() == columns());
    int n = rows();
    Matrix ret(n, n);
    for (int x = 0; x < n; x++) {
      T s = get(x, x);
      for (int i = 0; i < x; i++) s -= sqr(ret.get(i, x));
      if (s < 0.0) return Matrix();
      float d = sqrt(s);
      ret.set(x, x, d);
      float dinv = 1.0/d;
      for (int y = x + 1; y < n; y++) {
	s = get(x, y);
	for (int i = 0; i < y; i++) {
	  s -= ret.get(i, x) * ret.get(i, y);
	}
	ret.set(x, y, s * dinv);
      }
    }
    return ret;
  }
};

static inline Matrix<float> solve_nnls(const Matrix<float> &V,
				       const Matrix<float> &answers) {
  assert(answers.columns() == 1);
  assert(answers.rows() == V.rows());
#if 0
  int M = V.rows();
  int N = V.columns();
  int mda = M;
  double rnorm;
  double *A = new double[M * N];
  double *B = new double[M];
  double *X = new double[N];
  double *w = new double[N];
  double *zz = new double[M];
  int *index = new int[N];
  int mode;
  for (int y = 0; y < V.rows(); y++) {
    for (int x = 0; x < V.columns(); x++) {
      A[x * mda + y] = V.get(x, y);
    }
  }
  for (int i = 0; i < M; i++) {
    B[i] = answers.get(0, i);
  }
  
  nnls_c(A,
	 &mda,
	 &M,
	 &N,
	 B,
	 X,
	 &rnorm,
	 w,
	 zz,
	 index,
	 &mode);
  delete[] A;
  delete[] B;
  delete[] w;
  delete[] zz;
  delete[] index;
  Matrix<float> ret(1, N);
  for (int i = 0; i < N; i++) {
    ret.set(0, i, X[i]);
  }
  delete[] X;
  assert(mode == 1);
  return ret;
#else
  int M = V.rows();
  int N = V.columns();
  float *A = new float[M * N];
  float *B = new float[M];
  for (int y = 0; y < V.rows(); y++) {
    for (int x = 0; x < V.columns(); x++) {
      A[x * M + y] = V.get(x, y);
    }
  }
  for (int i = 0; i < M; i++) {
    B[i] = answers.get(0, i);
  }
  
  float *X = nnls(A, B, N, M);
  delete[] A;
  delete[] B;

  Matrix<float> ret(1, N);
  for (int i = 0; i < N; i++) {
    ret.set(0, i, X[i]);
  }
  free(X);
  return ret;
#endif
}


class NNLSInvertThreader : public Threader {
public:
NNLSInvertThreader(const Matrix<float> *self,
		   Matrix<float> *ret) :
  ysize_(self->rows()), self_(self), ret_(ret) {
    Run(ysize_);
  }
  virtual void DoTask(int x) {
    Matrix<float> ans(1, ysize_);
    ans.set(0, x, 1.0);
    Matrix<float> tmp = solve_nnls(*self_, ans);
//    Matrix<float> tmp = self_->fnnls(ans);
    for (int y = 0; y < tmp.rows(); y++) {
      ret_->set(x, y, tmp.get(0, y));
    }
  }
private:
  int ysize_;
  const Matrix<float> *self_;
  Matrix<float> *ret_;
};

static inline Matrix<float> invert_nnls(Matrix<float> m) {
  Matrix<float> ret(m.rows(), m.columns());
  NNLSInvertThreader inverter(&m, &ret);
  return ret;
}


class NNLSSolver {
public:
  virtual ~NNLSSolver() {}
  virtual Matrix<float> solve(Matrix<float> x) = 0;
};

class NNLSSolver_chol_fnnls : public NNLSSolver {
  
};

class NNLSSolver_simple : public NNLSSolver {
public:
  NNLSSolver_simple(Matrix<float> tmp) : tmp_(tmp) {}
  virtual Matrix<float> solve(Matrix<float> x) {
    return solve_nnls(tmp_, x);
  }
private:
  Matrix<float> tmp_;
};


template<class T> std::ostream& operator<<(std::ostream& os, const Matrix<T> m) {
  os<<std::endl;
  for(int y = 0; y < m.rows(); y++) {
    os << (y == 0 ? "[ " : "  ");  
    for(int x = 0; x < m.columns(); x++) {
      if(x != 0) os << ", ";
      os << m.get(x,y);
    }
    os << (y == m.rows() - 1 ? " ]\n" : ";\n");  
  }
  return os;
}

static void save_matrix(Matrix<float> data, FILE *f) {
  fprintf(f, "%d %d\n", data.columns(), data.rows());
  for (int y = 0; y < data.rows(); y++) {
    for (int x = 0; x < data.columns(); x++) {
      if (data.get(x, y) == 0.0) {
	fprintf(f, "0.0 ");
      } else {
	fprintf(f, "%.18f ", data.get(x, y));
      }
    }
    fprintf(f, "\n");
  }
}

static Matrix<float> read_matrix(const std::string &name) {
  FILE *f = fopen(name.c_str(), "r");
  if (f == NULL) {
    std::string tmp = "fopen matrix: " + name;
    perror(tmp.c_str());
    exit(1);
  }
  int xsize, ysize;
  assert(2 == fscanf(f, "%d %d", &xsize, &ysize));
  Matrix<float> data(xsize, ysize);
  for (int y = 0; y < data.rows(); y++) {
    for (int x = 0; x < data.columns(); x++) {
      float v;
      assert(fscanf(f, "%f", &v));
      data.set(x, y, v);
    }
  }
  fclose(f);
  return data;
}

#endif
