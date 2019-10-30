#ifndef COMMON_REF_H
#define COMMON_REF_H

// Refcounted pointers.
// Calls T->AddRef() / T->SubRef();
// Subref is assumed to handle any freeing needed.
template<class T>
class RefPtr {
public:
  RefPtr() : ptr_(nullptr) {}
  explicit RefPtr(T* p) : ptr_(p) { AddRef(); }
  ~RefPtr() { Free(); }
  RefPtr(RefPtr const& other) : ptr_(other.ptr_) { AddRef(); }
  RefPtr(RefPtr&& other) { ptr_ = other.ptr_; other.ptr_ = nullptr; }
  RefPtr& operator=(RefPtr const& other) {
    if (other.ptr_ != ptr_) {
      Free();
      ptr_ = other.ptr_;
      AddRef();
    }
    return *this;
  }

  void Free() { if (ptr_) { ptr_->SubRef(); ptr_ = nullptr; }  }
  T* operator->() {
    if (!ptr_) {
      STDOUT.print("NULL POINTER! @");
      STDOUT.println((long)__builtin_extract_return_addr(
		       __builtin_return_address(0)), HEX);
      delay(100);
    }
    return ptr_;
  }
  T* get() { return ptr_; }
  T& operator*() { return *ptr_; }
  bool operator==(const RefPtr& other) const { return ptr_ == other.ptr_; }
  bool operator!=(const RefPtr& other) const { return ptr_ == other.ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

private:
  void AddRef() { if (ptr_) ptr_->AddRef(); }
  T* ptr_;
};

#endif
