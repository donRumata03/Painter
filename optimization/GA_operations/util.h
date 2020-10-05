//
// Created by Vova on 29.09.2020.
//

#pragma once

#include <optimization/stroke_color_optimizing.h>
#include "data_representation/stroke.h"

/**
 * The returning buffer actually doesn`t have colors.
 * The strokes are marked as colored ror performance reasons.
 */
inline std::vector<colored_stroke> unpack_stroke_data_buffer(const std::vector<double>& stroke_data_buffer) {
	assert(sizeof(stroke) == 7 * sizeof(double)); // This format is assumed
	assert(!(stroke_data_buffer.size() % 7)); // Should be dividable by 7
	auto total_stroke_number = stroke_data_buffer.size() / 7;

	std::vector<colored_stroke> strokes;
	strokes.resize(total_stroke_number);

	for (size_t stroke_index = 0; stroke_index < total_stroke_number; ++stroke_index) {
		size_t stroke_value_initial_index = 7 * stroke_index;

		/*auto &unpacked_colored_stroke = */
		reinterpret_cast<stroke&>(strokes[stroke_index]) = stroke {
						{ stroke_data_buffer[stroke_value_initial_index],
						  stroke_data_buffer[stroke_value_initial_index + 1] },
						{ stroke_data_buffer[stroke_value_initial_index + 2],
						  stroke_data_buffer[stroke_value_initial_index + 3] },
						{ stroke_data_buffer[stroke_value_initial_index + 4],
						  stroke_data_buffer[stroke_value_initial_index + 5] },
						stroke_data_buffer[stroke_value_initial_index + 6]
				};

		// strokes[stroke_index].width = double{}; // <- The stroke color is undefined at this moment
	}

	return strokes;
}

inline void colorize_strokes (std::vector<colored_stroke>& strokes_to_colorize, const Image& image)
{
	for (auto& stroke : strokes_to_colorize) find_stroke_color(stroke, image);
};


template<
        class ColoredOrNotStroke,
        std::enable_if_t<std::is_same_v<ColoredOrNotStroke, stroke> or std::is_same_v<ColoredOrNotStroke, colored_stroke>, int*> nothing = nullptr
>
std::vector<double> pack_stroke_data(const std::vector<colored_stroke>& strokes) {
	auto stroke_number = strokes.size();
	std::vector<double> stroke_data_buffer(stroke_number);

	for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
		const auto& the_stroke = strokes[stroke_index];

		stroke_data_buffer[stroke_index]     = the_stroke.p0.x;
		stroke_data_buffer[stroke_index + 1] = the_stroke.p0.y;

		stroke_data_buffer[stroke_index + 2] = the_stroke.p1.x;
		stroke_data_buffer[stroke_index + 3] = the_stroke.p1.y;

		stroke_data_buffer[stroke_index + 4] = the_stroke.p2.x;
		stroke_data_buffer[stroke_index + 5] = the_stroke.p2.y;

		stroke_data_buffer[stroke_index + 7] = the_stroke.width;
	}

	return stroke_data_buffer;
}
