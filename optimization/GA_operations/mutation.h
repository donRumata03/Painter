//
// Created by Vova on 25.09.2020.
//

#pragma once

#include "common_operations/stroke_limits.h"

#include "optimization/GA_operations/util.h"

/**
 * Callable object for mutation in GA
 */
class mutator
{
	stroke_limit_descriptor limits;

	explicit mutator(const stroke_limit_descriptor& limits) : limits(limits)
	{
		//
	}

	void operator()(std::vector<double>& stroke_data_buffer, const std::vector<double>& sigmas, double target_gene_number, const normalizer& normaaa) {
		auto strokes = unpack_stroke_data_buffer(stroke_data_buffer);

	}

};
