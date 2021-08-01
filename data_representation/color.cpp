//
// Created by Vova on 13.07.2020.
//

#include "color.h"

color get_color_from_hex(const std::string& hex)
{
    std::stringstream ss("0x");
    uint32_t x;

    ss << hex;
    ss >> std::hex >> x;

    return color(((x & 0xFF0000u) >> 16u) / 255.,
                 ((x & 0x00FF00u) >> 8u) / 255.,
                 ((x & 0x0000FFu)) / 255.);
}

void to_json(json& j, const byte_color& col)
{
    j["r"] = col.r;
    j["g"] = col.g;
    j["b"] = col.b;
}

void from_json(const json& j, byte_color& col)
{
    assert(j.contains("r") and j.contains("g") and j.contains("b"));

    col = byte_color(j.at("r"), j.at("g"), j.at("b"));
}

