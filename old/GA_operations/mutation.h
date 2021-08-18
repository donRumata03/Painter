//
// Created by Vova on 25.09.2020.
//

#pragma once

#include "common_operations/stroke_limits.h"

#include "util.h"

/**
 * Callable object for mutation in GA
 */
class mutator
{
	stroke_limit_descriptor limits{};
	double move_probability = -1;

public:
	mutator() = default;

	explicit mutator(const stroke_limit_descriptor& limits, double move_probability) : limits(limits), move_probability(move_probability)
	{
		assert(move_probability >= 0 and move_probability <= 1);
	}

	void operator()(std::vector<double>& stroke_data_buffer, const std::vector<double>& sigmas, double target_gene_number, const normalizer& normaaa) const {
		auto strokes = unpack_stroke_data_buffer(stroke_data_buffer);

		if (pythonic_random() < move_probability) {
			// std::cout << stroke_data_buffer.size() << " " << sigmas.size() << std::endl;
			assert(sizeof(stroke) / sizeof(double) == 7);
			assert(stroke_data_buffer.size() == sigmas.size());

			double per_gene_move_probability = target_gene_number / double(stroke_data_buffer.size());

			for(auto& this_stroke : strokes) {
				bool move_or_not_to_move = pythonic_random() < per_gene_move_probability;
				if (!move_or_not_to_move) {
					// std::cout << "Not Move" << std::endl;
					continue;
				}
				// std::cout << "Move" << std::endl;


				// Move:
				double x_sigma = sigmas[0];
				double y_sigma = sigmas[1];

				double dx = normaaa.generate(0, x_sigma);
				double dy = normaaa.generate(0, y_sigma);

				shift_stroke(this_stroke, { dx, dy });
			}
		}
		else{
			// Randomly mutate:
			return GA::mutate(stroke_data_buffer, sigmas, target_gene_number, normaaa);
		}
	}


};
