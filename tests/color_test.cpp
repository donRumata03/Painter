#include <gtest/gtest.h>

#include "data/color.h"


TEST(Color, ToAndFromFloatingPoint) {
  ByteColor color = {255, 100, 1};
  auto float_color = to_floating_point<double, uint8_t>(color);
  auto byte_color = from_floating_point<uint8_t, double>(float_color);

  ASSERT_EQ(color, byte_color);
}
