//
// Created by Vova on 16.02.2021.
//

#include "annealing_worker.h"
#include <Annealing/annealing_optimize.h>


/// Custom constructor:
AnnealingWorker::AnnealingWorker (const Image& image, const CommonStrokingParams& stroking_params,
                                  const AnnealingStrokingParams& params, const fs::path& logging_path, bool enable_console_output)

                                  : annealing_stroking_params(params), common_stroking_params(stroking_params)
{
	image.copyTo(initial_image);

	image_w = initial_image.cols;
	image_h = initial_image.rows;

	this->enable_console_output = enable_console_output;

	configured_error_function = final_fitness_function(
            ImageStrokingData(image, stroking_params.use_constant_color, stroking_params.stroke_color),
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

	logger = OnImprovementUpdatingLogger(ImageStrokingData(image, stroking_params.use_constant_color, stroking_params.stroke_color),
                                      params.iterations, stroking_params.logging_percentage, logging_path, stroking_params.canvas_color, enable_console_output);
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
				custom_exp_temperature_dynamic(0.2),
				enable_console_output,
				nullptr,
				nullptr,
				nullptr,
				logger
	);
}

const std::vector<double>& AnnealingWorker::get_best_genome ()
{
	return result.best_genome;
}

void AnnealingWorker::show_fitness_dynamic ()
{
	auto current_ys = result.current_energy_dynamic;
	auto best_ys = result.best_energy_dynamic;

	std::vector<double> xs_to_plot(current_ys.size());
	std::iota(xs_to_plot.begin(), xs_to_plot.end(), 0.);

	add_vectors_to_plot(xs_to_plot, current_ys, { .name = "MSE Dynamic of current genome" });
	add_vectors_to_plot(xs_to_plot, best_ys, { .name = "MSE Dynamic of best genome since the beginning" });
	show_plot({ .log_y = true, .window_title =  "MSE Dynamic" });
}

//double AnnealingWorker::average_computation_time_seconds () const
//{
//	return configured_error_function.average_computation_time_seconds();
//}
//
//double AnnealingWorker::average_computation_time_per_pixel_seconds () const
//{
//	return configured_error_function.average_computation_time_per_pixel_seconds();
//}
//
//double AnnealingWorker::computations_performed () const
//{
//	return configured_error_function.computations_performed();
//}
//
//double AnnealingWorker::time_spent_counting () const
//{
//	return configured_error_function.time_spent_counting();
//}

//void AnnealingWorker::print_diagnostic_information ()
//{
//	std::cout
//			<< "Computations performed: " << computations_performed() << " (" << annealing_stroking_params.iterations << " expected)" << std::endl
//			<< "Average computational time: " << average_computation_time_seconds() * 1e+3 << "ms" << std::endl
//			<< "Computational time per pixel: " << average_computation_time_per_pixel_seconds() * 1e+9 << "ns" << std::endl
//			<< "=> Computational speed: " << size_t(std::round(1 / average_computation_time_per_pixel_seconds() / 1e+6)) << " MegaPixel / (sec * thread)"
//			<< std::endl;
//}

