//
// Created by Vova on 02.10.2020.
//

#include <optimization/GA_operations/stroke_range_generator.h>
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

	auto mutation_sigmas = generate_point_sigmas_for_stroke_genome(stroke_number, { double(image_w), double(image_h) }, 1. / 20, );

	GA_params ga_params {
		.population_size = 100,
		.epoch_num = 10,

		/// numeric params
//		double hyper_elite_fit_pow = 5;
//		double elite_fit_pow = 2;
//		double parent_fit_pow = 0.3;
		.hazing_percent = 0.6,

		/// Mutation:
		.mutation_percent_sigma = 0,
		.target_gene_mutation_number = stroke_number * 1.5, // Out of `stroke_number * 7`
		.cut_mutations = true,


			// 	GA::crossover_mode crossover_mode = GA::crossover_mode::low_variance_genetic;
		// 	std::optional<double> exiting_fitness_value = {};

		.custom_operations = operations,

//		bool allow_multithreading = false;
//		size_t threads = std::thread::hardware_concurrency() - 2;
	};

	// Configure error function counter:
	final_error_function configured_error_function(image, stroke_number, true);
	auto point_ranges = generate_point_ranges_for_stroke_genome(stroke_number, { double(image_w), double(image_h) });

	GA::ga_optimize(configured_error_function, point_ranges, ga_params);
}
