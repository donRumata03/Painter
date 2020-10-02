//
// Created by Vova on 29.09.2020.
//

#pragma once

#include "common_operations/basic_constraining.h"
#include "common_operations/stroke_limits.h"

struct final_constrainer
{
	stroke_limit_descriptor limits;

	explicit final_constrainer(const stroke_limit_descriptor& descriptor) : limits(descriptor)
	{
		//
	}

	void operator ()(GA::population& population, const std::vector<std::pair<double, double>>& constraints) {
		// Ignore given constraints
		for (const auto& genome : population) {
			auto strokes = unpack_stroke_data_buffer(genome);

			for (auto& stroke : strokes) {
				limits.constrain_stroke_to_requirements(stroke);
			}
		}
	}
};


