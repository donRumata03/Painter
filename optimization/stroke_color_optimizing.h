//
// Created by Vova on 17.07.2020.
//

#pragma once

#include "utils/image_io_utils.h"
#include "utils/stroke.h"
#include "error_computing.h"

color find_stroke_color(const stroke& colorless_stroke, const Image& image);
// ^^^ TODO: implement it somehow! (local optimization... after using dummy function)


color find_stroke_color_by_ariphmetic_mean(const stroke& colorless_stroke, const Image& image);
// ^^^ TODO
