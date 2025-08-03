#include <gtest/gtest.h>
#include <malloc.h>
#include <SimpleCPP/Pointer.h>

#include <algorithm>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

size_t allocated_count;
size_t allocated_size;

using simplecpp::Pointer;

void* alloc(const size_t& size) {
  allocated_count++;
  allocated_size += size;
  return malloc(size);
}

void dealloc(void* ptr) noexcept {
  allocated_count--;
  free(ptr);
}

class PointerTest : public testing::Test {
 protected:
  void SetUp() override {
    allocated_count = 0;
    allocated_size = 0;
  }
};

TEST_F(PointerTest, DefaultConstructor) {
  using type = float;

  Pointer<type, alloc, dealloc> ptr{};
  EXPECT_EQ(ptr.get_ref_count(), 0);
  EXPECT_EQ(allocated_count, 0);
}

TEST_F(PointerTest, LenConstructor) {
  using type = Pointer<float>;
  constexpr auto NUM_ELEMENTS = 32;
  constexpr auto ALLOCATION_SIZE = sizeof(type) * NUM_ELEMENTS;

  {
    Pointer<type, alloc, dealloc> ptr{NUM_ELEMENTS};

    EXPECT_EQ(ptr.get_ref_count(), 1);
    EXPECT_EQ(allocated_count, 1);
    EXPECT_EQ(allocated_size, ALLOCATION_SIZE);
  }

  EXPECT_EQ(allocated_count, 0);
}

TEST_F(PointerTest, ExistingDataConstructor) {
  using type = float;
  constexpr auto NUM_ELEMENTS = 32;
  constexpr auto ALLOCATION_SIZE = sizeof(type) * NUM_ELEMENTS;

  {
    std::mt19937 gen{NUM_ELEMENTS};
    std::normal_distribution<type> dist{};
    std::vector<type> data(NUM_ELEMENTS);
    std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });

    Pointer<type, alloc, dealloc> ptr{data.data(), NUM_ELEMENTS};

    EXPECT_EQ(ptr.get_ref_count(), 1);
    EXPECT_EQ(allocated_count, 1);
    EXPECT_EQ(allocated_size, ALLOCATION_SIZE);

    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
      EXPECT_EQ(ptr[i], data[i]);
    }
  }

  EXPECT_EQ(allocated_count, 0);
}

TEST_F(PointerTest, ExistingDataConstructorInvalidArg) {
  using type = float;
  constexpr auto NUM_ELEMENTS = 32;
  constexpr auto ALLOCATION_SIZE = sizeof(type) * NUM_ELEMENTS;

  {
    Pointer<type, alloc, dealloc> ptr;
    EXPECT_THROW((ptr = Pointer<type, alloc, dealloc>{nullptr, NUM_ELEMENTS}),
                 std::invalid_argument);
    EXPECT_EQ(ptr.get_ref_count(), 1);
    EXPECT_EQ(allocated_count, 1);
    EXPECT_EQ(allocated_size, ALLOCATION_SIZE);
  }

  EXPECT_EQ(allocated_size, 0);
  EXPECT_EQ(allocated_count, 0);
}

TEST_F(PointerTest, CopyConstructor) {
  using type = float;
  constexpr auto NUM_ELEMENTS = 32;
  constexpr auto ALLOCATION_SIZE = sizeof(type) * NUM_ELEMENTS;

  std::mt19937 gen{NUM_ELEMENTS};
  std::normal_distribution<type> dist{};
  std::vector<type> data(NUM_ELEMENTS);
  std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });

  Pointer<type, alloc, dealloc> ptr1{data.data(), NUM_ELEMENTS};
  {
    auto ptr2{ptr1};
    EXPECT_EQ(ptr2.get_ref_count(), 2);
    EXPECT_EQ(ptr1.get_ref_count(), 2);
    EXPECT_EQ(allocated_count, 1);
    EXPECT_EQ(allocated_size, ALLOCATION_SIZE);

    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
      EXPECT_EQ(ptr1[i], ptr2[i]);
    }
  }
  EXPECT_EQ(ptr1.get_ref_count(), 1);
}

TEST_F(PointerTest, MoveConstructor) {
  using type = float;
  constexpr auto NUM_ELEMENTS = 32;
  constexpr auto ALLOCATION_SIZE = sizeof(type) * NUM_ELEMENTS;

  std::mt19937 gen{NUM_ELEMENTS};
  std::normal_distribution<type> dist{};
  std::vector<type> data(NUM_ELEMENTS);
  std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });

  Pointer<type, alloc, dealloc> ptr1{data.data(), NUM_ELEMENTS};
  {
    auto ptr2{std::move(ptr1)};
    EXPECT_EQ(ptr2.get_ref_count(), 1);
    EXPECT_EQ(ptr1.get_ref_count(), 0);
    EXPECT_EQ(allocated_count, 1);
    EXPECT_EQ(allocated_size, ALLOCATION_SIZE);

    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
      EXPECT_EQ(ptr2[i], data[i]);
    }
  }
}

TEST_F(PointerTest, Destructor) {
  using type = float;
  constexpr auto NUM_ELEMENTS = 32;
  constexpr auto ALLOCATION_SIZE = sizeof(type) * NUM_ELEMENTS;

  {
    Pointer<type, alloc, dealloc> ptr{NUM_ELEMENTS};
  }

  EXPECT_EQ(allocated_count, 0);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE);
}