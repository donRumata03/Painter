//
// Created by Vova on 26.02.2021.
//

#pragma once

#include <io_api/image_io_utils.h>
#include "data_representation/stroke.h"
#include <rasterization/stroke_rasterizer.h>
#include <GA_operations/util.h>

#include <utility>


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

	ImageStrokingData imageData;

	color canvas_color {};

	fs::path base_path;
	fs::path updating_path;

	bool enable_console_output;

public:

	OnImprovementUpdatingLogger() = default;
	OnImprovementUpdatingLogger(ImageStrokingData imageData, size_t total_iterations, double logging_percentage, fs::path logging_path, color canvas_color, bool enable_console_output = true)
										: canvas_color(canvas_color), total_iterations(total_iterations), logging_fraction(logging_percentage), base_path(std::move(logging_path)), imageData(std::move(imageData)),
										enable_console_output(enable_console_output)
	{
		if (fs::exists(base_path)) fs::remove_all(base_path);

		fs::create_directories(base_path);
		save_image(this->imageData.image, base_path / "original.png");

		updating_path = base_path / "updates";
		fs::create_directories(updating_path);
	}


	void operator() (const std::vector<double>& stroke_set, size_t this_iteration, double error) const;
};


