#pragma once

#include "data/color.h"
#include "data/stroke.h"
#include "utils/image.h"


Color find_stroke_color_by_gradient_descent(const Stroke& colorless_stroke, const Image& image, double learning_rate,
                                            size_t iterations, bool log_debug_information = false);

Color find_stroke_color_by_ariphmetic_mean(const Stroke& colorless_stroke, const Image& image);


Color find_stroke_color(const Stroke& colorless_stroke, const Image& image);

void find_stroke_color(ColoredStroke& colored_stroke, const Image& image);