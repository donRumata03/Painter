//
// Created by Vova on 16.02.2021.
//

#include "annealing_worker.h"
#include <Annealing/annealing_optimize.h>


/// Custom constructor:
AnnealingWorker::AnnealingWorker (const Image& image, const CommonStrokingParams& stroking_params,
                                  const AnnealingStrokingParams& params, const fs::path& logging_path)

                                  : annealing_stroking_params(params), common_stroking_params(stroking_params)
{
	image.copyTo(initial_image);

	image_w = initial_image.cols;
	image_h = initial_image.rows;

	configured_error_function = final_fitness_function(
			image,
			common_stroking_params.stroke_number,
			true,
			true,
			common_stroking_params.canvas_color
	);

	configured_mutator = mutator();
}



/// Interface implementation:

void AnnealingWorker::run_one_iteration ()
{
	throw std::runtime_error("Can't run only one iteration for annealing!");
}

void AnnealingWorker::run_remaining_iterations ()
{
	/// Launch annealing:
	auto annealing_output = annealing_optimize();
}

const std::vector<double>& AnnealingWorker::get_best_genome ()
{
	return best_genome;
}

void AnnealingWorker::show_fitness_dynamic ()
{

}

double AnnealingWorker::average_computation_time_seconds () const
{
	return 0;
}

double AnnealingWorker::average_computation_time_per_pixel_seconds () const
{
	return 0;
}

double AnnealingWorker::computations_performed () const
{
	return 0;
}

void AnnealingWorker::print_diagnostic_information ()
{

}

