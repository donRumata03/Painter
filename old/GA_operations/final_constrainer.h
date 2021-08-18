//
// Created by Vova on 29.09.2020.
//

#pragma once

#include "common_operations/basic_constraining.h"
#include "common_operations/stroke_limits.h"

struct final_constrainer
{
	stroke_limit_descriptor limits {};

	final_constrainer() = default;

	explicit final_constrainer(const stroke_limit_descriptor& descriptor) : limits(descriptor)
	{
		//
	}

	void operator ()(GA::Population& population, const std::vector<std::pair<double, double>>& constraints) const {
		// Ignore given constraints
		for (auto& genome : population) {
			auto strokes = unpack_stroke_data_buffer(genome);

			for (auto& stroke : strokes) {
				limits.constrain_stroke_to_requirements(stroke);
			}
			// std::vector<double> stroke_data = pack_stroke_data(strokes);
			genome = pack_stroke_data(strokes); // std::move(stroke_data);
		}
	}
};


