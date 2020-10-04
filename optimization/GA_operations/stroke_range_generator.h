//
// Created by Vova on 02.10.2020.
//

#pragma once

#include "painter_pch.h"
#include "data_representation/stroke.h"

inline std::vector<std::pair<double, double>> generate_point_ranges_for_stroke_genome (
		size_t stroke_number, const point& image_size, const std::pair<double, double>& width_range)
{
	assert(sizeof(stroke) == 7 * sizeof(double));
	std::vector<std::pair<double, double>> res (stroke_number * 7);

	std::pair<double, double> x_range = { 0., double(image_size.x) };
	std::pair<double, double> y_range = { 0., double(image_size.y) };

	for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
		size_t base_buffer_index = stroke_index * 7;

		res[base_buffer_index + 0] = res[base_buffer_index + 2] = res[base_buffer_index + 4] = x_range;
		res[base_buffer_index + 1] = res[base_buffer_index + 3] = res[base_buffer_index + 5] = y_range;

		res[base_buffer_index + 6] = width_range;
	}

	return res;
}

inline std::vector<double> generate_point_sigmas_for_stroke_genome (size_t stroke_number, const point& image_size,
										double percent_sigma_for_coords, double absolute_sigma_for_width)
{
	assert(sizeof(stroke) == 7 * sizeof(double));
	std::vector<double> res(stroke_number * 7);

	double x_sigma = image_size.x * percent_sigma_for_coords;
	double y_sigma = image_size.y * percent_sigma_for_coords;
	// double width_sigma = ;

	for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
		size_t base_buffer_index = stroke_index * 7;

		// Coords:
		res[base_buffer_index + 0] = res[base_buffer_index + 2] = res[base_buffer_index + 4] = x_sigma;
		res[base_buffer_index + 1] = res[base_buffer_index + 3] = res[base_buffer_index + 5] = y_sigma;

		res[base_buffer_index + 6] = absolute_sigma_for_width;
	}

	return res;
}


