//
// Created by Vova on 27.09.2020.
//

#pragma once

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

class final_error_function
{
	const Image& initial_image;

	size_t total_stroke_number;
	size_t w, h;

	explicit final_error_function(const Image& image, size_t strokes) : initial_image(image), total_stroke_number(strokes) {
		w = image.cols;
		h = image.rows;
	}

	double operator()(const std::vector<double>& stroke_data_buffer) const
	{
		// !(stroke_data_buffer.size() % 6)
		assert(total_stroke_number * 7 == stroke_data_buffer.size()); // Should be dividable by 7 and the result should be equal to 7 * this->total_stroke_number (one double for each stuff)

		// Parse the stroke set and compute the colors for them:
		std::vector<colored_stroke> strokes;

		for (size_t stroke_index = 0; stroke_index < total_stroke_number; ++stroke_index) {
			size_t stroke_value_initial_index = 7 * stroke_index;

			auto& unpacked_colorless_stroke = strokes.emplace_back(
					stroke{
							{ stroke_data_buffer[stroke_value_initial_index], stroke_data_buffer[stroke_value_initial_index + 1] },
							{ stroke_data_buffer[stroke_value_initial_index + 2], stroke_data_buffer[stroke_value_initial_index +3] },
							{ stroke_data_buffer[stroke_value_initial_index + 4], stroke_data_buffer[stroke_value_initial_index +5] },
							stroke_data_buffer[stroke_value_initial_index + 6]
					},
					double {} // <- Color is undefined
			);

			find_stroke_color(unpacked_colorless_stroke, initial_image);

		}
	}

};


