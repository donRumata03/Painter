//
// Created by Vova on 26.02.2021.
//

#pragma once


/**
 * Logs on some of the updates depending on the given hyperparameters
 *
 * Regulation ideas:
 * 	1. Log everything
 * 	2. Log some fraction of all precedents
 * 	3. Make an inverse exponential plan (iteration-logging_speed) using probable amount pow and log only if it's below plan
 *
 *
 * Usage example: Annealing
 */
class OnImprovementUpdatingLogger
{
	size_t total_iterations = 0;
	double logging_fraction = 0.;
	fs::path base_path;


public:

	OnImprovementUpdatingLogger() = default;
	OnImprovementUpdatingLogger(size_t total_iterations, double logging_percentage, fs::path logging_path)
										: total_iterations(total_iterations), logging_fraction(logging_percentage), base_path(std::move(logging_path))
	{

	}


	void operator() (const std::vector<double>&, size_t this_iteration) const;
};


