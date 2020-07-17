//
// Created by Vova on 17.07.2020.
//

#pragma once

#include "utils/image_io_utils.h"
#include "utils/stroke.h"
#include "error_computing.h"

color find_stroke_color (const stroke &colorless_stroke, const Image &image, double learning_rate, size_t iterations);


color find_stroke_color_by_ariphmetic_mean(const stroke& colorless_stroke, const Image& image);


// Optimize stroke set:
// TODO!