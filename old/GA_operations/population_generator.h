//
// Created by Vova on 06.10.2020.
//

#pragma once

#include <common_operations/stroke_limits.h>
#include <common_operations/stroke_generation.h>
#include "util.h"

#include "genome_generator.h"

class final_population_generator
{
	stroke_limit_descriptor limits;
	size_t strokes_in_image;

	FinalGenomeGenerator m_generator;

public:
	final_population_generator() = default;
	explicit final_population_generator(const stroke_limit_descriptor& limits, size_t stroke_number)
					: limits(limits), strokes_in_image(stroke_number), m_generator(limits, stroke_number) {}

	std::vector<std::vector<double>> operator() (const std::vector<std::pair<double, double>>& ranges, size_t amount) const {
		std::vector<std::vector<double>> res(amount);
		// res.reserve(amount);

		for (size_t image_index = 0; image_index < amount; ++image_index) {
//			std::vector<stroke> this_image_strokes(strokes_in_image);
//
//			for (size_t stroke_index = 0; stroke_index < strokes_in_image; ++stroke_index) {
//				this_image_strokes[stroke_index] = generate_stroke(limits);
//			}
//
//			auto packed_strokes = pack_stroke_data(this_image_strokes);
			res[image_index] = m_generator(strokes_in_image * sizeof(stroke) / sizeof(double));  // std::move(packed_strokes);
		}


		return res;
	}

};


