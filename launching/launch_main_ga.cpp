//
// Created by Vova on 02.10.2020.
//

#include <GA_operations/callbacks/GA_informer.h>
#include "launch_main_ga.h"

void launch_the_GA (const std::string &filename)
{
	/// Constants:
/*
	size_t stroke_number = 100;
	size_t population_size = 10000;
	size_t epoch_num = 10;
	double stroke_length_to_image_size_fraction = 0.05;

	double coord_mutation_percent_sigma = 1. / 20;
	double stroke_mutation_to_stroke_width_factor = 2.;
*/
	/// Image opening; other constants:
//	Image image = open_image(filename);
//	// show_image_in_system_viewer(image);
//
//	size_t image_w = image.cols;
//	size_t image_h = image.rows;

//	double typical_coord = geometric_mean({ double(image_w), double(image_h) });
//	std::cout << "Typical Coord: " << typical_coord << std::endl;
//	double stroke_typical_size = typical_coord * stroke_width_size_fraction;
//	std::cout << "Stroke typical size: " << stroke_typical_size << std::endl;

	// Generate params:
//	stroke_limit_descriptor limits{
//		.min_dx = stroke_typical_size / 2,
//		.max_dx = stroke_typical_size * 2,
//		.min_dy = stroke_typical_size / 2,
//		.max_dy = stroke_typical_size * 2,
//		.min_width = -1, // TODO!
//		.max_width = -1, // TODO: width vs. length
//	};

	// GA::GA_operation_set operations;

	// final_constrainer configured_constrainer(limits);
	// operations.genome_constraint = configured_constrainer;

/*
	auto mutation_sigmas = generate_point_sigmas_for_stroke_genome(
			stroke_number,
			{ double(image_w), double(image_h) },
			coord_mutation_percent_sigma,
			stroke_typical_size * stroke_mutation_to_stroke_width_factor
	);
*/

/*
	GA_params ga_params {
		.population_size = population_size,
		.epoch_num = epoch_num,
		/// numeric params
//		double hyper_elite_fit_pow = 5;
//		double elite_fit_pow = 2;
//		double parent_fit_pow = 0.3;
		.hazing_percent = 0.6,
		/// Mutation:
		.mutation_percent_sigma = -1,
		.target_gene_mutation_number = stroke_number * 1.5, // Out of `stroke_number * 7`
		.cut_mutations = true,
		.individual_mutation_sigmas = mutation_sigmas,
		// 	GA::crossover_mode crossover_mode = GA::crossover_mode::low_variance_genetic;
		// 	std::optional<double> exiting_fitness_value = {};
		.custom_operations = operations,
		.allow_multithreading = true,
//		size_t threads = std::thread::hardware_concurrency() - 2;
	};
*/
	/// Configure error function counter:
	// final_fitness_function configured_error_function(image, stroke_number, true);
/*
	auto point_ranges = generate_point_ranges_for_stroke_genome(
			stroke_number,
			{ double(image_w), double(image_h) },
			{ typical_coord * stroke_width_size_fraction / 2, typical_coord * stroke_width_size_fraction * 2 }
	);
*/
	// std::vector<double> GA_fitnesses; GA_fitnesses.reserve(epoch_num);

	// GA::ga_optimize(configured_error_function, point_ranges, ga_params, GA_informer(image), &GA_fitnesses);

	// std::vector<double> xs_for_fitnesses(GA_fitnesses.size());
	// add_vectors_to_plot(GA_fitnesses, xs_for_fitnesses, { .name = "Fitness dynamic" });

	GA_launcher ga_launcher(filename);
	ga_launcher.configure_common_helpers();
	ga_launcher.configure_GA_operation_helpers();

	ga_launcher.run();
	ga_launcher.show_fitness_dynamic();

}

GA_launcher::GA_launcher (std::string filename)
							: filename(std::move(filename))
{
	image = open_image(this->filename);

	image_w = image.cols;
	image_h = image.rows;

	std::cout <<
		"Initialized GA from image: " <<  this->filename << ": " << console_colors::green << image_w << " x " << image_h << console_colors::remove_all_colors
	<< std::endl;

	typical_coord = geometric_mean({ double(image_w), double(image_h) });

	stroke_typical_length       = typical_coord         * stroke_length_to_image_size_fraction;
	stroke_typical_width        = stroke_typical_length * stroke_width_to_length_factor;

	stroke_coord_mutation_sigma = stroke_typical_length * stroke_coord_mutation_to_stroke_length_factor;
	stroke_width_mutation_sigma = stroke_typical_width  * stroke_width_mutation_to_stroke_width_factor;

	param_half_range = std::sqrt(stroke_param_relative_range);

	// Info:
	std::cout << console_colors::green << "Configuration info: " << console_colors::remove_all_colors << std::endl;
	std::cout << "Typical Coord: " << typical_coord << std::endl;

	std::cout << "Stroke typical length: " << stroke_typical_length << std::endl;
	std::cout << "Stroke typical width: " << stroke_typical_width << std::endl;

	std::cout << "Stroke coord mutation sigma: " << stroke_coord_mutation_sigma << std::endl;
	std::cout << "Stroke width mutation sigma: " << stroke_width_mutation_sigma << std::endl;

	std::cout << "____________________________________________________________" << std::endl;
}

void GA_launcher::configure_common_helpers ()
{
	limits = stroke_limit_descriptor{
			.min_dx     = stroke_typical_length / param_half_range,
			.max_dx     = stroke_typical_length * param_half_range,

			.min_dy     = stroke_typical_length / param_half_range,
			.max_dy     = stroke_typical_length * param_half_range,

			.min_width  = stroke_typical_width / param_half_range,
			.max_width  = stroke_typical_width * param_half_range,

			.image_rectangle = get_image_range_limits<double>(image)
	};

	std::cout << "[GA_launcher]: successfully initialized common parameters" << std::endl;
}

void GA_launcher::configure_GA_operation_helpers ()
{
	configured_constrainer = final_constrainer(limits);
	configured_generator = final_generator(limits, stroke_number);
	configured_crossover = final_crossover();
	configured_mutator = mutator(limits, move_mutation_probability);

	ga_operations.genome_constraint = configured_constrainer;
	ga_operations.population_generation = configured_generator;
	ga_operations.parents_matting = configured_crossover;
	ga_operations.mutation = configured_mutator;

	point_ranges = generate_point_ranges_for_stroke_genome(
			stroke_number,
			{ double(image_w), double(image_h) },
			{ stroke_typical_width / param_half_range, stroke_typical_width * param_half_range }
	);

	mutation_sigmas = generate_point_sigmas_for_stroke_genome(
			stroke_number,
			{ double(image_w), double(image_h) },
			stroke_coord_mutation_sigma,
			stroke_width_mutation_sigma
	);


	ga_params = {
			epoch_num,
			population_size,
			/// numeric params

			GA::hazing_GA_params {
				.hazing_percent = 0.8,
			},
			GA::mutation_GA_params {
				.mutation_percent_sigma = -1,
				.target_gene_mutation_number = stroke_number * 4., // Out of `stroke_number * 7`
				.cut_mutations = true,
				.individual_mutation_sigmas = mutation_sigmas,
			},
			GA::crossover_mode::low_variance_genetic,
			std::optional<double> {},
			GA::threading_GA_params {
				.allow_multithreading = false,
				.threads = std::thread::hardware_concurrency() - 2
			},

			ga_operations,

			GA::exception_policy::catch_and_log_fact,


//			.hazing_percent = 0.8,
//			/// Mutation:
//			.mutation_percent_sigma = -1,
//			.target_gene_mutation_number = stroke_number * 4., // Out of `stroke_number * 7`
//			.cut_mutations = true,
//			.individual_mutation_sigmas = mutation_sigmas,
//			// 	GA::crossover_mode crossover_mode = GA::crossover_mode::low_variance_genetic;
//			// 	std::optional<double> exiting_fitness_value = {};
//			.custom_operations = ga_operations,
//			.allow_multithreading = enable_multithreading,
//		size_t threads = std::thread::hardware_concurrency() - 2;
	};

	configured_fitness_function = new final_fitness_function{ image, stroke_number, !enable_multithreading, canvas_color };

	bool enable_detailed_logging = (logging_percentage != 0);
	logger = image_logging_callback(image, (fs::path{ painter_base_path} / "log/_latest").string(),
			logging_percentage, enable_detailed_logging);

	std::cout << "[GA_launcher]: successfully initialized GA-specific parameters" << std::endl;
}

void GA_launcher::run ()
{
	// this->GA_fitnesses.reserve(epoch_num + 1);

	std::cout << "[GA_launcher]: Running GA..." << std::endl;

	std::function<void(const GA::Population&, size_t, GA::logging_type)> logger_storage {logger};

	GA::ga_optimize(
			*configured_fitness_function,
			point_ranges,
			ga_params,
			GA_informer(image),
			&GA_fitnesses,
			&logger_storage
	);

	std::cout << "[GA_launcher]: Finished GA" << std::endl;
}

void GA_launcher::show_fitness_dynamic ()
{
	std::cout << "GA_fitnesses: " << GA_fitnesses << std::endl;

	std::vector<double> xs_for_fitnesses(GA_fitnesses.size());
	for (size_t x_index = 0; x_index < xs_for_fitnesses.size(); ++x_index) {
		xs_for_fitnesses[x_index] = double(x_index);
	}
	add_vectors_to_plot(xs_for_fitnesses, GA_fitnesses);
	show_plot({ .window_title = "Fitness dynamic" });
}
