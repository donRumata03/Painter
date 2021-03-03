//
// Created by Vova on 22.10.2020.
//

#include "GA_worker.h"




GA_worker::GA_worker (const Image& image, const CommonStrokingParams& common_parameters, const GA_stroking_parameters& ga_stroking_parameters, const fs::path& logging_path)
		: common_params(common_parameters), GA_params(ga_stroking_parameters) {
	image.copyTo(initial_image);

	image_w = image.cols;
	image_h = image.rows;

	/// Count typical distances:
//	typical_coord = geometric_mean({ double(image_w), double(image_h) });
//
//	stroke_typical_length       = typical_coord         * launch_params.stroke_length_to_image_size_fraction;
//	stroke_typical_width        = stroke_typical_length * launch_params.stroke_width_to_length_factor;
//
//	stroke_coord_mutation_sigma = stroke_typical_length * launch_params.stroke_coord_mutation_to_stroke_length_factor;
//	stroke_width_mutation_sigma = stroke_typical_width  * launch_params.stroke_width_mutation_to_stroke_width_factor;
//
//	param_half_range = std::sqrt(launch_params.stroke_param_relative_range);

	limits = generate_stroke_limits_by_raw_parameters(common_params, image_w, image_h);
//		stroke_limit_descriptor{
//			.min_dx     = stroke_typical_length / param_half_range,
//			.max_dx     = stroke_typical_length * param_half_range,
//
//			.min_dy     = stroke_typical_length / param_half_range,
//			.max_dy     = stroke_typical_length * param_half_range,
//
//			.min_width  = stroke_typical_width / param_half_range,
//			.max_width  = stroke_typical_width * param_half_range,
//
//			.image_rectangle = get_image_range_limits<double>(image)
//	};

	/// GA data:
	point_ranges = generate_point_ranges_by_raw_parameters(common_params, image_w, image_h);
//		generate_point_ranges_for_stroke_genome(
//			params.stroke_number,
//			{ double(image_w), double(image_h) },
//			{ stroke_typical_width / param_half_range, stroke_typical_width * param_half_range }
//	);

	mutation_sigmas = generate_point_sigmas_by_raw_parameters(common_params, image_w, image_h);
//		generate_point_sigmas_for_stroke_genome(
//			params.stroke_number,
//			{ double(image_w), double(image_h) },
//			stroke_coord_mutation_sigma,
//			stroke_width_mutation_sigma
//	);

	// std::cout << "[GA_worker]: made GA data" << std::endl;

	/// Init GA operation performers:
	configured_constrainer = final_constrainer(limits);
	configured_generator = final_population_generator(limits, common_params.stroke_number);
	configured_crossover = final_crossover();
	configured_mutator = mutator(limits, common_params.move_mutation_probability);

	ga_operations.genome_constraint = configured_constrainer;
	ga_operations.population_generation = configured_generator;
	ga_operations.parents_matting = configured_crossover;
	ga_operations.mutation = configured_mutator;

	// std::cout << "[GA_worker]: GA operations ready" << std::endl;

	configured_fitness_function = final_fitness_function{
			ImageStrokingData(image, common_params.use_constant_color, common_params.stroke_color),
			common_params.stroke_number,
			!GA_params.allow_multithreading,
			false,
			common_params.canvas_color
	};

	bool enable_detailed_logging = (common_params.logging_percentage != 0);
	logger = image_logging_callback(ImageStrokingData(image, common_params.use_constant_color, common_params.stroke_color),
                                 logging_path.string(),
                                 common_params.logging_percentage,
                                 enable_detailed_logging);

	// std::cout << "[GA_worker]: fitness and logger ready" << std::endl;


	/// GA params:
	ga_params = {
			GA_params.population_size,
			/// numeric params

			GA::hazing_GA_params {
					.hazing_percent = 0.8,
			},
			GA::mutation_GA_params {
					.mutation_percent_sigma = -1,
					.target_gene_mutation_number = common_params.stroke_number * 4., // Out of `stroke_number * 7`
					.cut_mutations = true,
					.individual_mutation_sigmas = mutation_sigmas,
			},
			GA::crossover_mode::low_variance_genetic,
			std::optional<double> {},
			GA::threading_GA_params {
					.allow_multithreading = GA_params.allow_multithreading,
					.threads = std::thread::hardware_concurrency() - 2
			},

			ga_operations,

			GA::exception_policy::catch_and_log_fact,
	};

	// std::cout << "[GA_worker]: GA params ready" << std::endl;


	optimizer.emplace(configured_fitness_function, point_ranges, ga_params);

	// std::cout << "[GA_worker]: inited optimizer" << std::endl;


	optimizer->plug_logger(logger);
	// std::cout << "[GA_worker]: plugged logger" << std::endl;


	optimizer->set_informer(GA_informer(image, GA_params.epoch_num));

	std::cout << "[GA_worker]: successfully initialized and ready to run" << std::endl;
}

void GA_worker::run_one_iteration ()
{
	optimizer->run_one_iteration(GA_params.epoch_num);
}

void GA_worker::run_remaining_iterations ()
{
	optimizer->run_many_iterations(
			GA_params.epoch_num - optimizer->iterations_processed(),
			GA_params.epoch_num
	);
}



void GA_worker::show_fitness_dynamic ()
{
	auto fhist = optimizer->get_fitness_history();
	std::cout << "GA fitness dynamic: " << fhist << std::endl;

	std::vector<double> xs_for_fitnesses(fhist.size());
	for (size_t x_index = 0; x_index < xs_for_fitnesses.size(); ++x_index) {
		xs_for_fitnesses[x_index] = double(x_index);
	}
	add_vectors_to_plot(xs_for_fitnesses, fhist);
	show_plot({ .window_title = "Fitness Dynamic" });
}

const std::vector<double>& GA_worker::get_best_genome ()
{
	return optimizer->get_best_genome();
}


