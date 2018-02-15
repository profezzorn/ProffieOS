#ifndef COMMON_LINKED_PTR_H
#define COMMON_LINKED_PTR_H

// Like a refcounted pointer, but uses a double-linked list
// to keep track of all references so that no refs are needed.

template<class T, class Free>
class LinkedPtr {
public:
  LinkedPtr() : next_(this), prev_(this) {}
  explicit LinkedPtr(T* p) : ptr_(p), next_(this), prev_(this) {}
  ~LinkedPtr() { leave(); }
  LinkedPtr(LinkedPtr const& other) { join(other); }
  LinkedPtr& operator=(LinkedPtr const& other) {
    if (&other != this) {
      leave();
      join(other);
    }
    return *this;
  }

  T* operator->() { return ptr_; }
  T& operator*() { return *ptr_; }
  bool operator==(const LinkedPtr& other) const { return ptr_ == other.ptr_; }
  bool operator!=(const LinkedPtr& other) const { return ptr_ == other.ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

private:
  void leave() {
    if (next_ == this) {
      Free(ptr_);
    } else {
      next_->prev_ = prev_;
      prev_->next_ = next_;
    }
  }
  void join(LinkedPtr const& other) {
    ptr_ = other.ptr_;
    prev_ = &other;
    next_ = other->next_;
    next_->prev_ = this;
    prev_->next_ = this;
  }

  T* ptr_;
  LinkedPtr* next_, *prev_;
};

#endif
