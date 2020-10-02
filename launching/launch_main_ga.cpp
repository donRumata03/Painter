//
// Created by Vova on 02.10.2020.
//

#include "launch_main_ga.h"

void launch_the_GA (const std::string &filename)
{
	// Constants:
	size_t stroke_number = 100;

	Image image = open_image(filename);
	// show_image_in_system_viewer(image);

	size_t image_w = image.cols;
	size_t image_h = image.rows;

	// Generate params:
	stroke_limit_descriptor limits{

	};

	GA::GA_operation_set operations;

	final_constrainer configured_constrainer(limits);
	operations.genome_constraint = configured_constrainer;

	GA_params ga_params {
		.population_size = 100,
		.epoch_num = 10,

	};

	// Configure error function counter:
	final_error_function configured_error_function(const Image& image, size_t strokes, is_run_sequentially);

	GA::ga_optimize();
}
