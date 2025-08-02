#include <gtest/gtest.h>
#include <malloc.h>
#include <SimpleCPP/Pointer.h>

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
  constexpr auto NUM_ELEMENTS = 32;
  {
    simplecpp::Pointer<simplecpp::Pointer<float>, alloc, dealloc> ptr{NUM_ELEMENTS};
  }
  EXPECT_EQ(allocated_count, 0);
  EXPECT_EQ(allocated_size, sizeof(simplecpp::Pointer<float>) * NUM_ELEMENTS);
}