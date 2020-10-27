//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "io_api/image_io_utils.h"
#include "data_representation/stroke.h"


double image_mse(const Image& image1, const Image& image2, bool parallel = false);

double stroke_mse (const Image& image, const colored_stroke& stroke, size_t step_number = 100, bool parallel = false);
double stroke_mse (const Image &image, const std::vector<stroke::Point> &stroke_points, const color &stroke_color, bool parallel = false);

