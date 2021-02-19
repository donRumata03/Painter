//
// Created by Vova on 06.10.2020.
//

#pragma once

#include <common_operations/stroke_limits.h>
#include <common_operations/stroke_generation.h>
#include "util.h"

class final_population_generator
{
	stroke_limit_descriptor limits;
	size_t stroke_number;

public:
	final_population_generator() = default;
	explicit final_population_generator(const stroke_limit_descriptor& limits, size_t stroke_number)
					: limits(limits), stroke_number(stroke_number) {}

	std::vector<std::vector<double>> operator() (const std::vector<std::pair<double, double>>& ranges, size_t amount) const {
		std::vector<std::vector<double>> res(amount);
		// res.reserve(amount);

		for (size_t image_index = 0; image_index < amount; ++image_index) {
//			auto& new_genome = res.emplace_back();
//			new_genome.reserve()
			std::vector<stroke> this_image_strokes(stroke_number);

			for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
				this_image_strokes[stroke_index] = generate_stroke(limits);
			}

			auto packed_strokes = pack_stroke_data(this_image_strokes);
			res[image_index] = std::move(packed_strokes);
		}

		// std::cout << "S" << res[0].size() << std::endl;

		return res;
	}

};


