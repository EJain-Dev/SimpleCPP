#ifndef SIMPLECPP_POINTER_H_
#define SIMPLECPP_POINTER_H_

#include <cstdlib>
#include <stdexcept>

namespace simplecpp {
void* default_allocator(const size_t& size) {
  auto data = malloc(size);
  if (data == nullptr) {
    throw std::bad_alloc();
  }
}

void default_deallocator(void* ptr) noexcept { free(static_cast<void*>(ptr)); }

/**
    @brief A smart pointer class that dynamically manages heap memory

    @tparam T The type of the data to be managed by the Pointer class
    @param alloc A custom allocator function that allocates the specified amount of bytes for the
   data. It should throw a std::bad_alloc exception if it fails to allocate memory or at least a
   exception if not std::bad_alloc.
    @param dealloc A custom deallocator function that frees the allocated memory
*/
template <typename T, void* (*alloc)(const size_t&) = default_allocator,
          void (*dealloc)(void*) noexcept = default_deallocator>
class Pointer {
 public:
  /**
   * @brief Default constructor to create an invalid Pointer object.
   */
  Pointer() noexcept : _refs(nullptr), _data(nullptr) {}

  /**
   * @brief Constructs a Pointer object with a specified length.
   *
   * @param len The length of the data to allocate.
   *
   * @exception std::bad_alloc if malloc returns null
   * @exception User defined exception if call to alloc fails with custom allocator. It may throw no
   * exception should the allocator not throw one.
   */
  explicit Pointer(const size_t& len)
      : _refs(static_cast<size_t*>(malloc(sizeof(size_t)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    if (_refs == nullptr) {
      throw std::bad_alloc();
    }
    *_refs = 1;
  }

  /**
   * @brief Constructs a Pointer object from an existing data array stored as a raw pointer.
   *
   * @param data Pointer to the start of the data array
   * @param len Length of the data array
   *
   * @exception std::invalid_argument if the provided data pointer is null.
   * @exception std::bad_alloc if malloc returns null
   * @exception User defined exception if call to alloc fails with custom allocator. It may throw no
   * exception should the allocator not throw one.
   *
   * @note This creates a COPY of the data for safety.
   */
  Pointer(const T* data, const size_t& len)
      : _refs(static_cast<size_t*>(malloc(sizeof(size_t)))),
        _data(static_cast<T*>(alloc(len * sizeof(T)))) {
    if (_refs == nullptr) {
      throw std::bad_alloc();
    }
    *_refs = 1;
    if (data == nullptr) {
      dec_ref();
      throw std::invalid_argument("A 'Pointer' object cannot be initialized with a null pointer.");
    }
    memcpy(_data, data, len * sizeof(T));
  }

  /**
   * @brief Copy constructor to create a Pointer object from another Pointer object.
   *
   * @param other The Pointer object to copy
   *
   * @note This is a shallow copy just like with raw pointers.
   */
  explicit Pointer(const Pointer& other) noexcept : _refs(other._refs), _data(other._data) {
    if (_refs != nullptr) {
      ++(*_refs);
    }
  }

  /**
   * @brief Move constructor to create a Pointer object from another Pointer object.
   *
   * @param other The Pointer object to move into this Pointer object
   *
   * @note This leaves the other Pointer object in a invalid state
   */
  explicit Pointer(Pointer&& other) noexcept : _refs(other._refs), _data(other._data) {
    other._refs = nullptr;
    other._data = nullptr;
  }

  /**
   * @brief Destroys the Pointer object
   */
  ~Pointer() noexcept { dec_ref(); }

  /**
   * @brief Copy operator to assign one Pointer object to another
   *
   * @param other The Pointer object to copy from
   *
   * @note This is a shallow copy just like with raw pointers.
   */
  Pointer& operator=(const Pointer& other) noexcept {
    if (this == &other) {
      return *this;
    }

    dec_ref();
    _refs = other._refs;
    if (_refs != nullptr) {
      ++(*_refs);
      _data = other._data;
    }

    return *this;
  }

  /**
   * @brief Move operator to assign one Pointer object to another
   *
   * @param other The Pointer object to move from
   *
   * @note This leaves the other Pointer object in an invalid state
   */
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

  /**
   * @brief Returns the underlying pointer object.
   *
   * @warning This is only for compatibility with C APIs and should not be used otherwise. Freeing
   * the pointer returned will result in undefined behavior for the lifetime of this object and upon
   * destruction.
   */
  T* get() noexcept { return _data; }

  /**
   * @brief Returns the underlying pointer object in a immutable state.
   *
   * @note This is only for compatibility with C APIs and should not be used otherwise.
   */
  const T* get() const noexcept { return _data; }
  /**
   * @brief Returns the reference count of the Pointer object.
   *
   * @note If this is an invalid Pointer object, it returns 0.
   */
  const size_t get_ref_count() const noexcept { return (is_valid()) ? (*_refs) : 0; }
  /**
   * @brief Checks if the Pointer object is valid (i.e., it points to allocated memory).
   *
   * @note The Pointer object may point to allocated memory but if it has decremented the reference
   * count and thus no longer shares the data it is not valid.
   */
  const bool is_valid() const noexcept { return _data != nullptr; }

  /**
   * @brief Indexing operator to access the data of the raw pointer at a specific index.
   */
  T& operator[](const size_t& idx) const noexcept { return _data[idx]; }
  /**
   * @brief Dereference operator to access the data at the pointer's location.
   */
  T& operator*() const noexcept { return *_data; }

  /**
   * @brief Equality operator that returns true if b is a copy of a or the reverse.
   *
   * @param a The first Pointer object to compare
   * @param b The second Pointer object to compare
   */
  friend bool operator==(const Pointer& a, const Pointer& b) noexcept {
    return a._refs == b._refs && a._data == b._data;
  }
  /**
   * @brief Equality operator that returns true if b is the underlying pointer maintained by a.
   *
   * @param a The Pointer object to compare
   * @param b A raw pointer
   */
  friend bool operator==(const Pointer& a, const T* b) noexcept { return a._data == b; }

  /**
   * @brief Checks if the raw pointer at a points to a address less than b
   *
   * @param a The first Pointer object to compare
   * @param b The second Pointer object to compare
   */
  friend bool operator<(const Pointer& a, const Pointer& b) noexcept { return a._data < b._data; }
  /**
   * @brief Checks if the raw pointer at a points to a address less than b
   *
   * @param a The Pointer object to compare
   * @param b A raw pointer
   */
  friend bool operator<(const Pointer& a, const T* b) noexcept { return a._data < b; }

  /**
   * @brief Checks if the raw pointer at a points to a address greater than b
   *
   * @param a The first Pointer object to compare
   * @param b The second Pointer object to compare
   */
  friend bool operator>(const Pointer& a, const Pointer& b) noexcept { return a._data > b._data; }
  /**
   * @brief Checks if the raw pointer at a points to a address greater than b
   *
   * @param a The Pointer object to compare
   * @param b A raw pointer
   */
  friend bool operator>(const Pointer& a, const T* b) noexcept { return a._data > b; }

 private:
  void dec_ref() noexcept {
    if (is_valid()) {
      if (--(*_refs) == 0) {
        dealloc(_data);
        free(_refs);
      }
      _refs = nullptr;
      _data = nullptr;
    }
  }

  size_t* _refs;
  T* _data;
};
}  // namespace simplecpp

#endif  // SIMPLECPP_POINTER_H_