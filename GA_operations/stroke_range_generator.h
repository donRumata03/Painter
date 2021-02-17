//
// Created by Vova on 02.10.2020.
//

#pragma once

#include "painter_pch.h"
#include "data_representation/stroke.h"
#include "launching/single_zone_workers/single_zone_worker.h"

/// For GA
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


/// For mutation:
inline std::vector<double> generate_point_sigmas_for_stroke_genome (size_t stroke_number, const point& image_size,
										double absolute_sigma_for_coords, double absolute_sigma_for_width)
{
	assert(sizeof(stroke) == 7 * sizeof(double));
	std::vector<double> res(stroke_number * 7);

	double x_sigma = absolute_sigma_for_coords;
	double y_sigma = absolute_sigma_for_coords;
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


/// Applying common parameters:


/// Generate stroke limits:

struct AppliedStroking {
	double typical_coord {};

	double stroke_typical_length {};
	double stroke_typical_width {};

	double stroke_coord_mutation_sigma {};
	double stroke_width_mutation_sigma {};

	double param_half_range {};
};



AppliedStroking apply_stroking_parameters(CommonStrokingParams params, size_t image_w, size_t image_h) {
	AppliedStroking res;

	/// Count typical distances:
	res.typical_coord = geometric_mean({ double(image_w), double(image_h) });

	res.stroke_typical_length       = res.typical_coord * params.stroke_length_to_image_size_fraction;
	res.stroke_typical_width        = res.stroke_typical_length * params.stroke_width_to_length_factor;

	res.stroke_coord_mutation_sigma = res.stroke_typical_length * params.stroke_coord_mutation_to_stroke_length_factor;
	res.stroke_width_mutation_sigma = res.stroke_typical_width * params.stroke_width_mutation_to_stroke_width_factor;

	res.param_half_range = std::sqrt(params.stroke_param_relative_range);

	return res;
}

stroke_limit_descriptor generate_stroke_limits_by_(CommonStrokingParams params, size_t w, size_t h) {
	auto applied = apply_stroking_parameters(params, w, h);

	auto limits = stroke_limit_descriptor{
			.min_dx     = applied.stroke_typical_length / applied.param_half_range,
			.max_dx     = applied.stroke_typical_length * applied.param_half_range,

			.min_dy     = applied.stroke_typical_length / applied.param_half_range,
			.max_dy     = applied.stroke_typical_length * applied.param_half_range,

			.min_width  = applied.stroke_typical_width / applied.param_half_range,
			.max_width  = applied.stroke_typical_width * applied.param_half_range,

			.image_rectangle = RangeRectangle<double>{ 0., double(w), 0., double(h) } // get_image_range_limits<double>(image)
	};

	return limits;
}

inline std::vector<std::pair<double, double>> generate_point_ranges_by_raw_parameters(CommonStrokingParams params, size_t w, size_t h) {
	auto applied = apply_stroking_parameters(params, w, h);

	return generate_point_ranges_for_stroke_genome(
			params.stroke_number,
			{ double(w), double(h) },
			{ applied.stroke_typical_width / applied.param_half_range, applied.stroke_typical_width * applied.param_half_range }
	);
}


inline std::vector<double> generate_point_sigmas_by_raw_parameters (CommonStrokingParams params, size_t w, size_t h)
{
	auto applied = apply_stroking_parameters(params, w, h);

	return generate_point_sigmas_for_stroke_genome(
			params.stroke_number,
			{ double(w), double(h) },
			applied.stroke_coord_mutation_sigma,
			applied.stroke_width_mutation_sigma
	);
}
