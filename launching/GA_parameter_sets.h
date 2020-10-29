//
// Created by Vova on 22.10.2020.
//

#pragma once

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
		.stroke_number = 10,
		.population_size = 5'00,
		.epoch_num = 20,

		.stroke_length_to_image_size_fraction = 0.4,
		.stroke_width_to_length_factor = 0.2,
		.stroke_coord_mutation_to_stroke_length_factor = 0.2,
		.stroke_width_mutation_to_stroke_width_factor = 0.2,

		.stroke_param_relative_range = 3,

		.move_mutation_probability = 0.2,
		.logging_percentage = 0.00,

		.canvas_color = { 0., 0., 0. },

		.allow_multithreading = true,
};

//////////

/// Splitting params:

struct image_splitting_params {
	size_t zones_x = 0;
	size_t zones_y = 0;

	double overlay_percent = 0;
};

inline image_splitting_params van_gogh_splitting_params {
	.zones_x = 2,
	.zones_y = 2,
	.overlay_percent = 0.1
};


