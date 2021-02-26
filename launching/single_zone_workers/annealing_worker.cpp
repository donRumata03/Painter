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

	auto stroke_limits = generate_stroke_limits_by_raw_parameters(stroking_params, image_w, image_h);

	configured_mutator = AdjustableGenomeMutator(
			stroke_limits,
			generate_point_sigmas_by_raw_parameters(stroking_params, image_w, image_h),
			common_stroking_params.stroke_number * double(sizeof(stroke) / sizeof(double)) * annealing_stroking_params.gene_mutation_fraction,
			common_stroking_params.move_mutation_probability,
			true // It will constrain the strokes before returning result
	); // mutator(stroke_limits, stroking_params.move_mutation_probability);
	configured_generator = FinalGenomeGenerator(stroke_limits, common_stroking_params.stroke_number);
}



/// Interface implementation:

void AnnealingWorker::run_one_iteration ()
{
	throw std::runtime_error("Can't run only one iteration for annealing!");
}

void AnnealingWorker::run_remaining_iterations ()
{
	/// Launch annealing:
	result = annealing_optimize<double, void>( // TODO: develop fast fitness function recounting
				configured_error_function,
				AnnealingOptimizeParameters{
					.iterations = annealing_stroking_params.iterations,
					.exiting_value = 0.,
					.typical_temperature = annealing_stroking_params.typical_temperature,
					.genes_in_genome = common_stroking_params.stroke_number * (sizeof(stroke) / sizeof(double)),
				},
				configured_generator,
				configured_mutator,
				default_exp_temperature_dynamic,
				true
				// TODO: add logger support and the logger itself!
	);
}

const std::vector<double>& AnnealingWorker::get_best_genome ()
{
	return result.best_genome;
}

void AnnealingWorker::show_fitness_dynamic ()
{
	auto ys_to_plot = result.current_energy_dynamic;

	std::vector<double> xs_to_plot(ys_to_plot.size());
	std::iota(xs_to_plot.begin(), xs_to_plot.end(), 0.);

	add_vectors_to_plot(xs_to_plot, ys_to_plot);
	show_plot();
}

double AnnealingWorker::average_computation_time_seconds () const
{
	return configured_error_function.average_computation_time_seconds();
}

double AnnealingWorker::average_computation_time_per_pixel_seconds () const
{
	return configured_error_function.average_computation_time_per_pixel_seconds();
}

double AnnealingWorker::computations_performed () const
{
	return configured_error_function.computations_performed();
}

void AnnealingWorker::print_diagnostic_information ()
{
	std::cout
			<< "Computations performed: " << computations_performed() << " (" << annealing_stroking_params.iterations << " expected)" << std::endl
			<< "Average computational time: " << average_computation_time_seconds() * 1e+3 << "ms" << std::endl
			<< "Computational time per pixel: " << average_computation_time_per_pixel_seconds() * 1e+9 << "ns" << std::endl
			<< "=> Computational speed: " << size_t(std::round(1 / average_computation_time_per_pixel_seconds() / 1e+6)) << " MegaPixel / (sec * thread)"
			<< std::endl;
}

