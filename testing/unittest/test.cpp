#include "gtest/gtest.h"

#include <stdio.h>

TEST(Test, hello)
{
  EXPECT_TRUE(false);
  fprintf(stderr, "gtest setup");
}