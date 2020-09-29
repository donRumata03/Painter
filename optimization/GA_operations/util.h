//
// Created by Vova on 29.09.2020.
//

#pragma once

#include "data_representation/stroke.h"

/**
 * The returning buffer actually doesn`t have colors.
 * The strokes are marked as colored ror performance reasons.
 */
inline std::vector<colored_stroke> unpack_stroke_data_buffer(const std::vector<double>& stroke_data_buffer) {
	assert(sizeof(stroke) / sizeof(double) == 7); // This format is assumed
	assert(!(stroke_data_buffer.size() % 7)); // Should be dividable by 7
	auto total_stroke_number = stroke_data_buffer.size() / 7;

	std::vector<colored_stroke> strokes;
	strokes.reserve(total_stroke_number);

	for (size_t stroke_index = 0; stroke_index < total_stroke_number; ++stroke_index) {
		size_t stroke_value_initial_index = 7 * stroke_index;

		/*auto &unpacked_colored_stroke = */
		strokes.emplace_back(
				stroke {
						{ stroke_data_buffer[stroke_value_initial_index],
						  stroke_data_buffer[stroke_value_initial_index + 1] },
						{ stroke_data_buffer[stroke_value_initial_index + 2],
						  stroke_data_buffer[stroke_value_initial_index + 3] },
						{ stroke_data_buffer[stroke_value_initial_index + 4],
						  stroke_data_buffer[stroke_value_initial_index + 5] },
						stroke_data_buffer[stroke_value_initial_index + 6]
				},
				double {} // <- The stroke color is undefined at this moment
		);
	}

	return strokes;
}
