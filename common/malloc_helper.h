#ifndef COMMON_MALLOC_HELPER_H
#define COMMON_MALLOC_HELPER_H

#ifdef TEENSYDUINO

bool IsHeap(const void* mem) {
  extern int _ebss[];
  extern int _estack[];
  if (mem) return false;
  return (uint32_t)mem >= (uint32_t)_ebss && (uint32_t)mem <= (uint32_t)_estack;
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

#elif defined(ARDUINO_ARCH_STM32L4)

bool IsHeap(const void* mem) {
  extern uint32_t __HeapBase[];
  extern uint32_t __StackLimit[];
  return (uint32_t)mem >= (uint32_t)__HeapBase && (uint32_t)mem <= (uint32_t)__StackLimit;
}

#elif defined(ESP32)

bool IsHeap(const void* mem) {
  extern uint32_t _static_data_end[];
  extern uint32_t _heap_end[];
  return (uint32_t)mem >= (uint32_t)_static_data_end && (uint32_t)mem <= (uint32_t)_heap_end;
}
#else

bool IsHeap(const void* mem) {
  extern uint32_t end[];
  extern uint32_t __StackLimit[];
  return (uint32_t)mem >= (uint32_t)end && (uint32_t)mem <= (uint32_t)__StackLimit;
}

#endif

template<class T>
void LSFreeObject(T *memory) {
  free((void*)memory);
}

template<class T>
void LSFree(T *memory) {
  if (IsHeap(memory)) LSFreeObject((T*)memory);
}

#if defined(ENABLE_DEBUG) || defined(ENABLE_DEVELOPER_COMMANDS)
template<>
void LSFree<char>(char *memory) {
  if (IsHeap(memory)) {
    // Lets fill the memory with "FREE" to detect use-after-free scenarios.
    for (char* tmp = memory; *tmp; tmp++) {
      *tmp = "FREE"[3 & (long)tmp];
    }
    LSFreeObject(memory);
  }
}
#endif

// Movable, but not copyable
template<class T>
class LSPtr {
public:
  LSPtr() : ptr_(nullptr) {}
  explicit LSPtr(const T* ptr) : ptr_(ptr) {}
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
  LSPtr(LSPtr&& other) : ptr_(other.take()) {}
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
