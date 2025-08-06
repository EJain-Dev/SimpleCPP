#include <gtest/gtest.h>
#include <malloc.h>
#include <SimpleCPP/pointer.h>

#include <exception>
#include <utility>

using type = float;
constexpr size_t ALLOC_SIZE = sizeof(type) + sizeof(size_t);
constexpr type val = 3;

size_t alloc_count;
size_t alloc_size;
size_t dealloc_count;

void* alloc(const size_t& size) {
  alloc_size += size;
  ++alloc_count;
  auto data = malloc(size);
  if (data == nullptr) {
    throw std::bad_alloc();
  }
  return data;
}

void dealloc(void* data) noexcept { free(data); }

using ptr = simplecpp::Pointer<type, alloc, dealloc>;

class PointerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    alloc_count = 0;
    alloc_size = 0;
    dealloc_count = 0;
  }
};

TEST_F(PointerTest, DefaultConstructor) {
  ptr p{};

  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(alloc_size, ALLOC_SIZE);
  EXPECT_EQ(dealloc_count, 0);
  EXPECT_TRUE(p.is_valid());
  const auto ref_count = p.get_ref_count();
  EXPECT_EQ(ref_count, 1);

  EXPECT_NE(p.get(), nullptr);
}

TEST_F(PointerTest, MainConstructor) {
  ptr p{val};

  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(alloc_size, ALLOC_SIZE);
  EXPECT_EQ(dealloc_count, 0);
  EXPECT_TRUE(p.is_valid());
  const auto ref_count = p.get_ref_count();
  EXPECT_EQ(ref_count, 1);
  EXPECT_EQ(*p, val);

  EXPECT_NE(p.get(), nullptr);
}

TEST_F(PointerTest, CopyConstructor) {
  ptr p{val};
  ptr p2{p};

  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(alloc_size, ALLOC_SIZE);
  EXPECT_EQ(dealloc_count, 0);
  EXPECT_TRUE(p2.is_valid());
  const auto ref_count = p.get_ref_count();
  EXPECT_EQ(ref_count, 2);
  EXPECT_EQ(*p, *p2);
  EXPECT_EQ(p.get(), p2.get());
}

TEST_F(PointerTest, MoveConstructor) {
  ptr p{val};
  ptr p2{std::move(p)};

  EXPECT_EQ(alloc_count, 1);
  EXPECT_EQ(alloc_size, ALLOC_SIZE);
  EXPECT_EQ(dealloc_count, 0);

  EXPECT_EQ(p.is_valid(), false);
  auto ref_count = p.get_ref_count();
  EXPECT_EQ(ref_count, 0);
  EXPECT_EQ(p.get(), nullptr);

  EXPECT_TRUE(p2.is_valid());
  ref_count = p2.get_ref_count();
  EXPECT_EQ(ref_count, 1);
  EXPECT_EQ(*p2, val);

  EXPECT_NE(p2.get(), nullptr);
}

TEST_F(PointerTest, Destructor) {
  ptr p{};
  p.~Pointer();
  EXPECT_FALSE(p.is_valid());
  const auto ref_count = p.get_ref_count();
  EXPECT_EQ(ref_count, 0);
  EXPECT_EQ(p.get(), nullptr);
}

TEST_F(PointerTest, ComparisonOperators) {
  ptr p{};
  ptr p2{p};
  ptr p3{};

  EXPECT_TRUE(p == p2);
  EXPECT_FALSE(p != p2);
  EXPECT_TRUE(p == p2.get());
  EXPECT_FALSE(p != p2.get());

  if (p3.get() > p2.get()) {
    EXPECT_TRUE(p3 > p2);
    EXPECT_TRUE(p3 > p2.get());
  } else {
    EXPECT_TRUE(p3 < p2);
    EXPECT_TRUE(p3 < p2.get());
  }
}