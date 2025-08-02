#ifndef SIMPLECPP_POINTER_H_
#define SIMPLECPP_POINTER_H_

#include <atomic>
#include <concepts>
#include <cstdlib>
#include <stdexcept>

namespace simplecpp {
template <typename T>
T* default_allocator(const size_t& len) {
  return static_cast<T*>(malloc(len * sizeof(T)));
}

template <typename T>
void default_deallocator(T* ptr) {
  free(static_cast<void*>(ptr));
}

template <typename T, T* (*alloc)(size_t), void (*dealloc)(T*)>
class Pointer {
 public:
  Pointer() = default;

  Pointer(const size_t& len)
      : _refs(static_cast<std::atomic<size_t>*>(alloc(sizeof(std::atomic<size_t>)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    _refs->store(1ZU);
  }

  Pointer(const T* _data, const size_t& len)
      : _refs(static_cast<std::atomic<size_t>*>(malloc(sizeof(std::atomic<size_t>)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    _refs->store(1ZU);
    if (_data == nullptr) {
      throw std::invalid_argument("A 'Pointer' object cannot be initialized with a null pointer.");
    }
  }

  Pointer(const Pointer& other) : _refs(other._refs), _data(other._data) { ++(*_refs); }

  Pointer(Pointer&& other) noexcept : _refs(other._refs), _data(other._data) {
    other._refs = nullptr;
    other._data = nullptr;
  }

  ~Pointer() { dec_ref(); }

  Pointer& operator=(const Pointer& other) {
    if (this == &other) {
      return *this;
    }

    dec_ref();
    _refs = other._refs;
    ++(*_refs);
    _data = other._data;

    return *this;
  }

  Pointer& operator=(Pointer&& other) noexcept {
    if (this == &other) {
      return *this;
    }

    dec_ref();
    _refs = other._refs;
    _data = other._data;
    other._refs = nullptr;
    other._data = nullptr;

    return *this;
  }

  operator T*() const noexcept { return _data; }

  T* get() const noexcept { return _data; }

  const size_t& get_ref_count() const noexcept { return *_refs; }

 private:
  void dec_ref() {
    if (--(*_refs) == 0) {
      dealloc(_refs);
      dealloc(_data);
    }
  }

  std::atomic<size_t>* _refs;
  T* _data;
};

template <typename T>
Pointer<T, default_allocator<T>, default_deallocator<T>>;
}  // namespace simplecpp

#endif  // SIMPLECPP_POINTER_H_