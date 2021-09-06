#include "color.h"


Color get_color_from_hex(const std::string& hex) {
  std::stringstream ss("0x");
  uint32_t x;

  ss << hex;
  ss >> std::hex >> x;

  return Color(((x & 0xFF0000u) >> 16u) / 255.,
               ((x & 0x00FF00u) >> 8u) / 255.,
               ((x & 0x0000FFu)) / 255.);
}

void to_json(json& j, const ByteColor& col) {
  j["r"] = col.r;
  j["g"] = col.g;
  j["b"] = col.b;
}

void from_json(const json& j, ByteColor& col) {
  assert(j.contains("r") and j.contains("g") and j.contains("b"));

  col = ByteColor(j.at("r"), j.at("g"), j.at("b"));
}

