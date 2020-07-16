//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "utils/image_io_utils.h"
#include "utils/stroke.h"


double image_mse(const Image& image1, const Image& image2);
double stroke_mse(const Image& image, const stroke& stroke, size_t step_number = 100);