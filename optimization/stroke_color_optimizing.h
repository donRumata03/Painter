//
// Created by Vova on 17.07.2020.
//

#pragma once

#include "io_api/image_io_utils.h"
#include "data_representation/stroke.h"
#include "optimization/error/error_computing.h"

color find_stroke_color_by_gradient_descent (
		const stroke &colorless_stroke, const Image &image, double learning_rate, size_t iterations,
		bool log_debug_information = false
);

color find_stroke_color_by_ariphmetic_mean(const stroke& colorless_stroke, const Image& image);


color find_stroke_color (const stroke& colorless_stroke, const Image& image);
void find_stroke_color (colored_stroke& colored_stroke, const Image& image);
void find_stroke_color (colored_stroke& colored_stroke, const color& setup_color);


