//
// Created by Vova on 22.10.2020.
//

#include "GA_worker.h"


GA_worker::GA_worker (const Image &image, const GA_launching_params &params) : launch_params(params) {
	image.copyTo(initial_image);

	image_w = image.cols;
	image_h = image.rows;

	/// Count typical distances:
	typical_coord = geometric_mean({ double(image_w), double(image_h) });

	stroke_typical_length       = typical_coord         * launch_params.stroke_length_to_image_size_fraction;
	stroke_typical_width        = stroke_typical_length * launch_params.stroke_width_to_length_factor;

	stroke_coord_mutation_sigma = stroke_typical_length * launch_params.stroke_coord_mutation_to_stroke_length_factor;
	stroke_width_mutation_sigma = stroke_typical_width  * launch_params.stroke_width_mutation_to_stroke_width_factor;

	param_half_range = std::sqrt(launch_params.stroke_param_relative_range);

	limits = stroke_limit_descriptor{
			.min_dx     = stroke_typical_length / param_half_range,
			.max_dx     = stroke_typical_length * param_half_range,

			.min_dy     = stroke_typical_length / param_half_range,
			.max_dy     = stroke_typical_length * param_half_range,

			.min_width  = stroke_typical_width / param_half_range,
			.max_width  = stroke_typical_width * param_half_range,

			.image_rectangle = get_image_range_limits<double>(image)
	};
}

void GA_worker::run_one_iteration ()
{
	optimizer->run_one_iteration(launch_params.epoch_num);
}

void GA_worker::run_remaining_iterations ()
{
	optimizer->run_many_iterations(
			launch_params.epoch_num - optimizer->iterations_processed(),
			launch_params.epoch_num
	);
}
