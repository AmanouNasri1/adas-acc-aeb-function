#include <gtest/gtest.h>
#include "acc/dummy.hpp"

TEST(Dummy, AddWorks) {
  EXPECT_EQ(acc::add(2, 3), 5);
}