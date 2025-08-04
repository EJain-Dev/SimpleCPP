#ifndef SIMPLECPP_POINTER_H_
#define SIMPLECPP_POINTER_H_

#include <cstdlib>
#include <stdexcept>

namespace simplecpp {
void* default_allocator(const size_t& size) noexcept { return malloc(size); }

void default_deallocator(void* ptr) noexcept { free(static_cast<void*>(ptr)); }

template <typename T, void* (*alloc)(const size_t&) = default_allocator,
          void (*dealloc)(void*) noexcept = default_deallocator>
class Pointer {
 public:
  Pointer() noexcept : _refs(nullptr), _data(nullptr) {}

  explicit Pointer(const size_t& len)
      : _refs(static_cast<size_t*>(malloc(sizeof(size_t)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    *_refs = 1;
  }

  Pointer(const T* data, const size_t& len)
      : _refs(static_cast<size_t*>(malloc(sizeof(size_t)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    *_refs = 1;
    if (data == nullptr) {
      dec_ref();
      throw std::invalid_argument("A 'Pointer' object cannot be initialized with a null pointer.");
    }
    memcpy(_data, data, len * sizeof(T));
  }

  explicit Pointer(const Pointer& other) noexcept : _refs(other._refs), _data(other._data) {
    ++(*_refs);
  }

  explicit Pointer(Pointer&& other) noexcept : _refs(other._refs), _data(other._data) {
    other._refs = nullptr;
  }

  ~Pointer() noexcept { dec_ref(); }

  Pointer& operator=(const Pointer& other) noexcept {
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

    return *this;
  }

  T* get() noexcept { return _data; }
  const T* get() const noexcept { return _data; }
  const size_t get_ref_count() const noexcept { return (is_valid()) ? (*_refs) : 0; }
  const bool& is_valid() const noexcept { return _refs == nullptr; }
  void make_null() noexcept {
    dec_ref();
    free(_refs);
    _refs = nullptr;
    _data = nullptr;
  }

  const T& operator[](const size_t& idx) const noexcept { return _data[idx]; }
  const T& operator*() const noexcept { return *_data; }

  friend bool operator==(const Pointer& a, const Pointer& b) noexcept {
    return a._refs == b._refs && a._data == b._data;
  }
  friend bool operator==(const Pointer& a, const T* b) noexcept { return a._data == b; }

  friend bool operator<(const Pointer& a, const Pointer& b) noexcept { return a._data < b._data; }
  friend bool operator<(const Pointer& a, const T* b) noexcept { return a._data < b; }

  friend bool operator>(const Pointer& a, const Pointer& b) noexcept { return a._data > b._data; }
  friend bool operator>(const Pointer& a, const T* b) noexcept { return a._data > b; }

 private:
  void dec_ref() noexcept {
    if (_refs != nullptr && *_refs > 0 && --(*_refs) == 0) {
      dealloc(_data);
      free(_refs);
      _refs = nullptr;
    }
  }

  size_t* _refs;
  T* _data;
};
}  // namespace simplecpp

#endif  // SIMPLECPP_POINTER_H_