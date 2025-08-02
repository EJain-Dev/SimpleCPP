#ifndef SIMPLECPP_POINTER_H_
#define SIMPLECPP_POINTER_H_

#include <atomic>
#include <concepts>
#include <cstdlib>
#include <stdexcept>

namespace simplecpp {
void* default_allocator(const size_t& size) { return malloc(size); }

void default_deallocator(void* ptr) { free(static_cast<void*>(ptr)); }

template <typename T, void* (*alloc)(const size_t&) = default_allocator,
          void (*dealloc)(void*) = default_deallocator>
class Pointer {
 public:
  Pointer() = default;

  Pointer(const size_t& len)
      : _refs(static_cast<std::atomic<size_t>*>(malloc(sizeof(std::atomic<size_t>)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    _refs->store(1);
  }

  Pointer(const T* data, const size_t& len)
      : _refs(static_cast<std::atomic<size_t>*>(malloc(sizeof(std::atomic<size_t>)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    _refs->store(1);
    if (data == nullptr) {
      throw std::invalid_argument("A 'Pointer' object cannot be initialized with a null pointer.");
    }
    memcpy(_data, data, len * sizeof(T));
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

  const size_t& get_ref_count() const noexcept { return _refs->load(); }

 private:
  void dec_ref() {
    if (_refs != nullptr) {
      if (--(*_refs) == 0) {
        free(_refs);
        dealloc(_data);
      }
    }
  }

  std::atomic<size_t>* _refs = nullptr;
  T* _data = nullptr;
};
}  // namespace simplecpp

#endif  // SIMPLECPP_POINTER_H_