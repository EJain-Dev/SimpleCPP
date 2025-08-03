#ifndef SIMPLECPP_POINTER_H_
#define SIMPLECPP_POINTER_H_

#include <atomic>
#include <concepts>
#include <cstdlib>
#include <stdexcept>

namespace simplecpp {
void* default_allocator(const size_t& size) noexcept { return malloc(size); }

void default_deallocator(void* ptr) noexcept { free(static_cast<void*>(ptr)); }

template <typename T, void* (*alloc)(const size_t&) noexcept = default_allocator,
          void (*dealloc)(void*) noexcept = default_deallocator>
class Pointer {
 public:
  Pointer() noexcept
      : _refs(static_cast<std::atomic<size_t>*>(malloc(sizeof(std::atomic<size_t>)))),
        _data(nullptr) {
    _refs->store(0);
  }

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
      _refs->store(0);
      throw std::invalid_argument("A 'Pointer' object cannot be initialized with a null pointer.");
    }
    memcpy(_data, data, len * sizeof(T));
  }

  Pointer(const Pointer& other) noexcept : _refs(other._refs), _data(other._data) { ++(*_refs); }

  Pointer(Pointer&& other) noexcept : _refs(other._refs), _data(other._data) {
    other._refs->store(0);
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
    other._data = nullptr;

    return *this;
  }

  const T* get() const noexcept { return _data; }
  const size_t get_ref_count() const noexcept { return _refs->load(); }

  const T& operator[](const size_t& idx) const noexcept { return _data[idx]; }
  const T& operator*() const noexcept { return *_data; }

  friend bool operator==(const Pointer& a, const Pointer& b) noexcept { return a._data == b._data; }

 private:
  void dec_ref() noexcept {
    if (_refs->load() != 0) {
      if (--(*_refs) == 0) {
        free(_refs);
        dealloc(_data);
      }
    }
  }

  std::atomic<size_t>* _refs;
  T* _data;
};
}  // namespace simplecpp

#endif  // SIMPLECPP_POINTER_H_