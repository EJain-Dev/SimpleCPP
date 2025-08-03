#include <gtest/gtest.h>
#include <malloc.h>
#include <SimpleCPP/Pointer.h>

#include <algorithm>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

size_t alloc_count;
size_t dealloc_count;
size_t allocated_size;

using simplecpp::Pointer;
using type = float;
constexpr auto NUM_ELEMENTS = 32;
constexpr auto ALLOCATION_SIZE = sizeof(type) * NUM_ELEMENTS;

void* alloc(const size_t& size) {
  alloc_count++;
  allocated_size += size;
  return malloc(size);
}

void dealloc(void* ptr) noexcept {
  dealloc_count++;
  free(ptr);
}

class PointerTest : public testing::Test {
 protected:
  void SetUp() override {
    alloc_count = 0;
    dealloc_count = 0;
    allocated_size = 0;
  }
};

TEST_F(PointerTest, DefaultConstructor) {
  Pointer<type, alloc, dealloc> ptr{};
  EXPECT_EQ(ptr.get_ref_count(), 0);
  EXPECT_EQ(alloc_count, 0);
  EXPECT_EQ(dealloc_count, 0);
}

TEST_F(PointerTest, LenConstructor) {
  Pointer<type, alloc, dealloc> ptr{NUM_ELEMENTS};

  EXPECT_EQ(ptr.get_ref_count(), 1);
  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(dealloc_count, 0);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE);
}

TEST_F(PointerTest, ExistingDataConstructor) {
  std::mt19937 gen{NUM_ELEMENTS};
  std::normal_distribution<type> dist{};
  std::vector<type> data(NUM_ELEMENTS);
  std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });

  Pointer<type, alloc, dealloc> ptr{data.data(), NUM_ELEMENTS};

  EXPECT_EQ(ptr.get_ref_count(), 1);
  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(dealloc_count, 0);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE);

  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    EXPECT_EQ(ptr[i], data[i]);
  }
}

TEST_F(PointerTest, ExistingDataConstructorInvalidArg) {
  Pointer<type, alloc, dealloc> ptr;
  try {
    ptr = Pointer<type, alloc, dealloc>{nullptr, NUM_ELEMENTS};
    FAIL()
        << "Expected std::invalid_argument when existing data constructor is called with nullptr";
  } catch (std::invalid_argument& e) {
    EXPECT_EQ(ptr.get_ref_count(), 0);
    EXPECT_EQ(alloc_count, 1);
    EXPECT_EQ(dealloc_count, 1);
    EXPECT_EQ(allocated_size, ALLOCATION_SIZE);
  }
}

TEST_F(PointerTest, CopyConstructor) {
  std::mt19937 gen{NUM_ELEMENTS};
  std::normal_distribution<type> dist{};
  std::vector<type> data(NUM_ELEMENTS);
  std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });

  Pointer<type, alloc, dealloc> ptr1{data.data(), NUM_ELEMENTS};
  auto ptr2{ptr1};
  EXPECT_EQ(ptr2.get_ref_count(), 2);
  EXPECT_EQ(ptr1.get_ref_count(), 2);
  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(dealloc_count, 0);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE);

  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    EXPECT_EQ(ptr1[i], ptr2[i]);
  }
}

TEST_F(PointerTest, MoveConstructor) {
  std::mt19937 gen{NUM_ELEMENTS};
  std::normal_distribution<type> dist{};
  std::vector<type> data(NUM_ELEMENTS);
  std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });

  Pointer<type, alloc, dealloc> ptr1{data.data(), NUM_ELEMENTS};
  auto ptr2{std::move(ptr1)};
  EXPECT_EQ(ptr2.get_ref_count(), 1);
  EXPECT_EQ(ptr1.get_ref_count(), 0);
  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(dealloc_count, 0);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE);

  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    EXPECT_EQ(ptr2[i], data[i]);
  }
}

TEST_F(PointerTest, Destructor) {
  {
    Pointer<type, alloc, dealloc> ptr{NUM_ELEMENTS};
    {
      auto ptr2{ptr};
    }
    EXPECT_EQ(ptr.get_ref_count(), 1);
    EXPECT_EQ(dealloc_count, 0);
  }

  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(dealloc_count, 1);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE);
}

TEST_F(PointerTest, CopyOperator) {
  std::mt19937 gen{NUM_ELEMENTS};
  std::normal_distribution<type> dist{};
  std::vector<type> data(NUM_ELEMENTS);
  std::vector<type> data2(NUM_ELEMENTS);
  std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });
  std::ranges::generate(data2, [&gen, &dist]() { return dist(gen); });

  Pointer<type, alloc, dealloc> ptr1{data.data(), NUM_ELEMENTS};
  Pointer<type, alloc, dealloc> ptr2{data2.data(), NUM_ELEMENTS};
  ptr2 = ptr1;
  EXPECT_EQ(ptr2.get_ref_count(), 2);
  EXPECT_EQ(ptr1.get_ref_count(), 2);
  EXPECT_EQ(alloc_count, 2);
  EXPECT_EQ(dealloc_count, 1);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE * 2);

  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    EXPECT_EQ(ptr1[i], ptr2[i]);
  }
}

TEST_F(PointerTest, MoveOperator) {
  std::mt19937 gen{NUM_ELEMENTS};
  std::normal_distribution<type> dist{};
  std::vector<type> data(NUM_ELEMENTS);
  std::vector<type> data2(NUM_ELEMENTS);
  std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });
  std::ranges::generate(data2, [&gen, &dist]() { return dist(gen); });

  Pointer<type, alloc, dealloc> ptr1{data.data(), NUM_ELEMENTS};
  Pointer<type, alloc, dealloc> ptr2{data2.data(), NUM_ELEMENTS};
  ptr2 = std::move(ptr1);
  EXPECT_EQ(ptr2.get_ref_count(), 1);
  EXPECT_EQ(ptr1.get_ref_count(), 0);
  EXPECT_EQ(alloc_count, 2);
  EXPECT_EQ(dealloc_count, 1);
  EXPECT_EQ(allocated_size, ALLOCATION_SIZE * 2);

  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    EXPECT_EQ(ptr2[i], data[i]);
  }
}

TEST_F(PointerTest, EqualityOperator) {
  Pointer<type> ptr1{NUM_ELEMENTS};
  auto ptr2{ptr1};
  Pointer<type> ptr3{NUM_ELEMENTS};
  EXPECT_EQ(ptr1 == ptr2, true);
  EXPECT_EQ(ptr3 != ptr2, true);
  EXPECT_EQ(ptr3 != ptr1, true);
  EXPECT_EQ(ptr1 == ptr2.get(), true);
  EXPECT_EQ(ptr3.get() != ptr2, true);
  EXPECT_EQ(ptr1.get() != ptr3, true);
  ptr2 = Pointer<type>{std::move(ptr1)};
  EXPECT_EQ(ptr1 != ptr2, true);
}