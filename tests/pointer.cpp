#include <gtest/gtest.h>
#include <malloc.h>
#include <SimpleCPP/Pointer.h>

#include <algorithm>
#include <random>#include <ranges>
#include <vector>

size_t allocated_count;
size_t allocated_size;

void* alloc(const size_t& size) {
  allocated_count++;
  allocated_size += size;
  return malloc(size);
}

void dealloc(void* ptr) {
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

TEST_F(PointerTest, LenConstructor) {
  using type = simplecpp::Pointer<float>;
  constexpr auto NUM_ELEMENTS = 32;

  {
    simplecpp::Pointer<type, alloc, dealloc> ptr{NUM_ELEMENTS};
  }

  EXPECT_EQ(allocated_count, 0);
  EXPECT_EQ(allocated_size, sizeof(type) * NUM_ELEMENTS);
}

TEST_F(PointerTest, ExistingDataConstructor) {
  using type = float;
  constexpr auto NUM_ELEMENTS = 32;

  {
    std::mt19937 gen{NUM_ELEMENTS};
    std::normal_distribution<type> dist{};
    std::vector<type> data(NUM_ELEMENTS);
    std::ranges::generate(data, [&gen, &dist]() { return dist(gen); });
    simplecpp::Pointer<type, alloc, dealloc> ptr{data.data(), NUM_ELEMENTS};
    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
      EXPECT_EQ(ptr[i], data[i]);
    }
  }

  EXPECT_EQ(allocated_count, 0);
  EXPECT_EQ(allocated_size, sizeof(type) * NUM_ELEMENTS);
}