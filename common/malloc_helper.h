#ifndef COMMON_MALLOC_HELPER_H
#define COMMON_MALLOC_HELPER_H

#ifdef TEENSYDUINO

bool IsHeap(const void* mem) {
  extern unsigned long _ebss;
  extern unsigned long _estack;
  if (mem) return false;
  return (uint32_t)mem >= _ebss && (uint32_t)mem <= _estack;
}

#elif defined(PROFFIE_TEST)
bool IsHeap(const void* mem) {
#ifdef __APPLE__
  return false; // leak
#else
  extern uint32_t end[];
  return (void*)mem >= (void*)&end;
#endif
}

#else

bool IsHeap(const void* mem) {
  extern uint32_t __HeapBase[];
  extern uint32_t __StackLimit[];
  return (uint32_t)mem >= (uint32_t)__HeapBase && (uint32_t)mem <= (uint32_t)__StackLimit;
}
#endif

template<class T>
void LSFreeObject(T *memory) {
//  STDOUT.print("FREE ");
//  STDOUT.println((uint32_t)memory, HEX);
  free((void*)memory);
}

template<class T>
void LSFree(T *memory) {
  if (IsHeap(memory)) LSFreeObject((T*)memory);
}

// Movable, but not copyable
template<class T>
class LSPtr {
public:
  LSPtr() : ptr_(nullptr) {}
  ~LSPtr() { Free(); }
  void Free() { LSFree(ptr_); }
  const T* get() const { return ptr_; }
  const T* take() { const T* ret = ptr_; ptr_ = nullptr; return ret; }
  void set(const T* t) {
    Free();
    ptr_ = t;
  }
    
  LSPtr& operator=(const T* t) {
    set(t);
    return *this;
  }
  // move assignment
  LSPtr& operator=(LSPtr&& other) {
    std::swap(ptr_, other.ptr_);
    return *this;
  }
  // move constructor
  LSPtr(LSPtr&& other) {
    set(other.take());
  }
private:
  LSPtr(LSPtr&); // prevent copy constructor
  LSPtr& operator=(const LSPtr&); // prevent copy assignment
  
  const T* ptr_;
};

const char* mkstr(const char* str) {
  int len = strlen(str);
  char* ret = (char*)malloc(len + 1);
  if (!ret) return "";
  memcpy(ret, str, len + 1);
  return ret;
}

template<class T>
class StaticWrapper {
public:
  using pointer = typename std::add_pointer<T>::type;

  template<class... Args>
    StaticWrapper(Args && ...args) {
    new (mData) T(std::forward<Args>(args)...);
  }
  
  pointer operator ->() {
    return reinterpret_cast<pointer>(mData);
  }
  
private:
  alignas(T) int8_t mData[sizeof(T)];
};

#endif
