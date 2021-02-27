//
// Created by Vova on 26.02.2021.
//

#pragma once

#include <io_api/image_io_utils.h>
#include "data_representation/stroke.h"
#include <rasterization/stroke_rasterizer.h>
#include <GA_operations/util.h>


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

	Image image;

	fs::path base_path;
	fs::path updating_path;

public:

	OnImprovementUpdatingLogger() = default;
	OnImprovementUpdatingLogger(Image image, size_t total_iterations, double logging_percentage, fs::path logging_path)
										: total_iterations(total_iterations), logging_fraction(logging_percentage), base_path(std::move(logging_path)), image(std::move(image))
	{
		if (fs::exists(base_path)) fs::remove_all(base_path);

		fs::create_directories(base_path);
		save_image(this->image, base_path / "original.png");

		updating_path = base_path / "updates";
		fs::create_directories(updating_path);
	}


	void operator() (const std::vector<double>& stroke_set, size_t this_iteration) const;
};


