#ifndef COMMON_LINKED_PTR_H
#define COMMON_LINKED_PTR_H

// Like a refcounted pointer, but uses a double-linked list
// to keep track of all references so that no refs are needed.

template<class T, class Free>
class LinkedPtr {
public:
  LinkedPtr() : ptr_(nullptr), prev_(this), next_(this) {}
  explicit LinkedPtr(T* p) : ptr_(p), prev_(this), next_(this) {}
  ~LinkedPtr() { leave(); }
  LinkedPtr(LinkedPtr const& other) { join(other); }
  LinkedPtr& operator=(LinkedPtr const& other) {
    if (&other != this) {
      leave();
      join(other);
    }
    return *this;
  }
  LinkedPtr& operator=(T* other) {
    leave();
    ptr_ = other;
    return *this;
  }

  T* get() { return ptr_; }
  T* operator->() { return ptr_; }
  T& operator*() { return *ptr_; }
  bool operator==(const LinkedPtr& other) const { return ptr_ == other.ptr_; }
  bool operator!=(const LinkedPtr& other) const { return ptr_ != other.ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

private:
  void leave() {
    if (next_ == this) {
      Free::Free(ptr_);
    } else {
      next_->prev_ = prev_;
      prev_->next_ = next_;
    }
  }
  void join(LinkedPtr const& other) {
    ptr_ = other.ptr_;
    prev_ = &other;
    next_ = other.next_;
    next_->prev_ = this;
    prev_->next_ = this;
  }

  T* ptr_;
  mutable LinkedPtr const *prev_;
  mutable LinkedPtr const *next_;
};

#endif
