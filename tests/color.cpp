#include <gtest/gtest.h>

#include "data/color.h"


TEST(ColorTest, ConvertToFloatingPoint) {
  ByteColor color = {255, 100, 1};
  auto float_color = to_floating_point<double, uint8_t>(color);
  auto byte_color = from_floating_point<uint8_t, double>(float_color);

  ASSERT_EQ(color, byte_color);
}

TEST(ColorTest, ConvertToJson) {
  ByteColor byte_color = {255, 100, 1};
  json j_byte_color = byte_color;
  auto converted_byte_color = j_byte_color.get<ByteColor>();

  ASSERT_EQ(byte_color, converted_byte_color);


  Color color = {1, 0.5, 0.25};
  json j_color = color;
  auto converted_color = j_color.get<Color>();

  ASSERT_EQ(color, converted_color);
}