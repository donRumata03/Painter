//
// Created by Vova on 02.10.2020.
//

#include "launch_main_ga.h"

#include "GA_worker.h"

#include "GA_parameter_sets.h"

void launch_the_GA (const std::string &filename)
{
/*

	GA_launcher ga_launcher(filename);
	ga_launcher.configure_common_helpers();
	ga_launcher.configure_GA_operation_helpers();

	ga_launcher.run();
	ga_launcher.show_fitness_dynamic();

*/
	Image image = open_image(filename);

	GA_launching_params this_params = one_stroke_params;

	GA_worker worker(image, this_params);
	worker.run_remaining_iterations();

	/// For one thread:
	// Average computational time: 1.96496ms
	// Computational time per pixel: 13.0997 (ns/pix)
	// => Speed is 77 MegaPix / (sec * thread)

	/// For many threads without memory allocation saving system:
	// Average computational time: 8.70384ms
	// Computational time per pixel: 58.0256ns
	// => Computational speed: 17 MegaPixel / (sec * thread)

	/// For many threads with memory allocation saving system (measured WITH buffer getting):
	// 31 MegaPixel / (sec * thread)

	/// For many threads with memory allocation saving system (measured WITHOUT buffer getting):
	// 31 MegaPixel / (sec * thread)

	std::cout
		<< "Computations performed: " << worker.computations_performed() << "(" << this_params.epoch_num * this_params.population_size << " expected)" << std::endl
		<< "Average computational time: " << worker.average_computation_time_seconds() * 1e+3 << "ms" << std::endl
		<< "Computational time per pixel: " << worker.average_computation_time_per_pixel_seconds() * 1e+9 << "ns" << std::endl
		<< "=> Computational speed: " << size_t(std::round(1 / worker.average_computation_time_per_pixel_seconds() / 1e+6)) << " MegaPixel / (sec * thread)"
	<< std::endl;

	worker.show_fitness_dynamic();
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
			// epoch_num,
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
				.allow_multithreading = true,
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

	configured_fitness_function = final_fitness_function{ image, stroke_number, !enable_multithreading, canvas_color };

	bool enable_detailed_logging = (logging_percentage != 0);
	logger = image_logging_callback(image, (fs::path{ painter_base_path} / "log/_latest").string(),
			logging_percentage, enable_detailed_logging);

	std::cout << "[GA_launcher]: successfully initialized GA-specific parameters" << std::endl;
}

void GA_launcher::run ()
{
	// this->GA_fitnesses.reserve(epoch_num + 1);

	std::cout << "[GA_launcher]: Running GA..." << std::endl;

	// std::function<void(const GA::Population&, size_t, GA::logging_type)> logger_storage {logger};

//	GA::ga_optimize(
//			*configured_fitness_function,
//			point_ranges,
//			ga_params,
//			GA_informer(image),
//			&GA_fitnesses,
//			&logger_storage
//	);

	GA::GA_optimizer optimizer(configured_fitness_function, point_ranges, ga_params);
	optimizer.set_informer(GA_informer(image, epoch_num));
	optimizer.plug_logger(logger);

	////////////////////////////////////////////////////////////////////////
	optimizer.run_many_iterations(epoch_num, epoch_num);
	////////////////////////////////////////////////////////////////////////

	// Get best fitness and genome: ... = optimizer.get_current_fitness(), ... = ...
	// Get fitness history:
	GA_fitnesses = optimizer.get_fitness_history();

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
