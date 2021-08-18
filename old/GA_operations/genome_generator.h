//
// Created by Vova on 19.02.2021.
//

#pragma once

#include <common_operations/stroke_limits.h>
#include <common_operations/stroke_generation.h>
#include "util.h"



class FinalGenomeGenerator
{
	stroke_limit_descriptor limits;
	size_t strokes_in_image;

public:
	FinalGenomeGenerator() = default;
	explicit FinalGenomeGenerator(const stroke_limit_descriptor& limits, size_t stroke_number)
			: limits(limits), strokes_in_image(stroke_number) {}

	std::vector<double> operator() (size_t amount) const
	{
		// Amount isn't the the number of strokes, it's the amount of pieces
		assert(not(amount % (sizeof(stroke) / sizeof(double))));

		size_t number_of_strokes = amount / 7;
		assert(number_of_strokes == strokes_in_image);

		std::vector<stroke> strokes(strokes_in_image);
		std::generate(strokes.begin(), strokes.end(), [this] () { return generate_stroke(limits); });
//
//		for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
//			strokes[stroke_index] = generate_stroke(limits);
//		}

		return pack_stroke_data(strokes);
	}
};


