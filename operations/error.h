#pragma once

#include "data/stroke.h"
#include "utils/image.h"


double image_mse(const Image& image1, const Image& image2, bool parallel = false);

double stroke_mse(const Image& image, const ColoredStroke& stroke, size_t step_number = 100, bool parallel = false);

double stroke_mse(const Image& image, const std::vector<Point>& stroke_points, const Color& stroke_color,
                  bool parallel = false);