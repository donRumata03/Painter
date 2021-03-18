//
// Created by Vova on 18.03.2021.
//

#pragma once

#include <common_operations/stroke_sorting.h>
#include <launching/SvgZoneLauncher.h>

#include <launching/GA_parameter_sets.h>
#include "_test_common_utils.h"


inline void test_stroke_sorting_function(const fs::path& sample_image_path) {
	/// Somehow get the strokes:
	// SvgZoneLauncher<AnnealingOptimizeParameters> launcher(sample_image_path, default_stroking_parameters, default_annealing_params, {  });
	// launcher.

	Image initial_image = sample_image_path;

	auto sorted_strokes = sort_strokes();


	visualize_stroke_painting(sorted_strokes);
}

