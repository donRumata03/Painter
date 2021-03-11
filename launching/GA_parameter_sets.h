//
// Created by Vova on 22.10.2020.
//

#pragma once

#include <launching/single_zone_workers/annealing_worker.h>
#include <launching/single_zone_workers/GA_worker.h>


inline auto default_stroking_parameters = CommonStrokingParams{
		.stroke_number = 100,

		.stroke_length_to_image_size_fraction = 0.2, // 0.2,
		.stroke_width_to_length_factor = 0.3,
		.stroke_coord_mutation_to_stroke_length_factor = 0.4,
		.stroke_width_mutation_to_stroke_width_factor = 0.4,

		.stroke_param_relative_range = 3,

		.move_mutation_probability = 0.2,
		.logging_percentage = 0.05,
		
		.canvas_color = { 0., 0., 0. }
};


inline auto default_GA_params = GA_stroking_parameters {
		.population_size = 200,
		.epoch_num = 50,

		.allow_multithreading = true,
};

inline auto default_annealing_params = AnnealingStrokingParams {
		.iterations = 800,

		.typical_temperature = 8e-4,
		.gene_mutation_fraction = 0.2,
};

/*
inline auto circle_30stroke_params = GA_launching_params {
		.stroke_number = 30,
		.population_size = 1'000,
		.epoch_num = 100,

		.stroke_length_to_image_size_fraction = 0.15,
		.stroke_width_to_length_factor = 0.5,
		.stroke_coord_mutation_to_stroke_length_factor = 0.5,
		.stroke_width_mutation_to_stroke_width_factor = 0.5,

		.stroke_param_relative_range = 3,

		.move_mutation_probability = 0.2,
		.logging_percentage = 0.00,

		.canvas_color = { 0., 0., 0. },

		.allow_multithreading = false,
};


inline auto one_stroke_params = GA_launching_params {
		.stroke_number = 3,
		.population_size = 5'000,
		.epoch_num = 20,

		.stroke_length_to_image_size_fraction = 0.4,
		.stroke_width_to_length_factor = 0.2,
		.stroke_coord_mutation_to_stroke_length_factor = 0.2,
		.stroke_width_mutation_to_stroke_width_factor = 0.2,

		.stroke_param_relative_range = 3,

		.move_mutation_probability = 0.2,
		.logging_percentage = 0.00,

		.canvas_color = { 0., 0., 0. },

		.allow_multithreading = false,
};

///////////

inline auto van_gogh_params = GA_launching_params {
		.stroke_number = 100,
		.population_size = 100,
		.epoch_num = 20,

		.stroke_length_to_image_size_fraction = 0.4, // 0.2,
		.stroke_width_to_length_factor = 0.3,
		.stroke_coord_mutation_to_stroke_length_factor = 0.4,
		.stroke_width_mutation_to_stroke_width_factor = 0.4,

		.stroke_param_relative_range = 3,

		.move_mutation_probability = 0.2,
		.logging_percentage = 0.00,

		.canvas_color = { 0., 0., 0. },

		.allow_multithreading = true,
};
*/

template<class OptimizerType>
typename OptimizerType::ParametersType get_default_special_params();

template<>
GA_stroking_parameters get_default_special_params<GA_worker>() { return default_GA_params; }
template<>
AnnealingStrokingParams get_default_special_params<AnnealingWorker>() { return default_annealing_params; }

template <class OptimizationAlgorithm>
struct CanBeParallelized {};

template<> struct CanBeParallelized<AnnealingWorker> {
	static constexpr auto value = false;
};

template<> struct CanBeParallelized<GA_worker> {
	static constexpr auto value = true;
};

//////////

/// Splitting params:

struct image_splitting_params {
	size_t zones_x = 0;
	size_t zones_y = 0;

	double overlay_percent = 0;
};

inline image_splitting_params van_gogh_splitting_params {
	.zones_x = 5,
	.zones_y = 10,
	.overlay_percent = 0.3
};


