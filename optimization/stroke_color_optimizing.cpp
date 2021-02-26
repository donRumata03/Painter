//
// Created by Vova on 17.07.2020.
//

#include "stroke_color_optimizing.h"

#include "optimization/error/error_computing.h"
#include <other_optimization/local_optimization.h>

color find_stroke_color_by_gradient_descent (const stroke &colorless_stroke, const Image &image, double learning_rate, size_t iterations,
                                             bool log_debug_information)
{
	color initial_color = find_stroke_color_by_ariphmetic_mean(colorless_stroke, image);
	auto stroke_points = colorless_stroke.get_points(100, get_image_range_limits(image));
	std::vector<double> initial_color_component_sequence = {
			initial_color.r,
			initial_color.g,
			initial_color.b
	};

	double initial_error = stroke_mse(image, stroke_points, initial_color);


	double little_color_delta = 1e-6;

	// Generate function for computing error function derivative:
	auto error_function_gradient_counter =
		[&](const std::vector<double>& current_color_vector, double current_error_function = -1.) -> std::vector<double>
	{

		color current_color(current_color_vector);

		// auto this_colored_stroke = colored_stroke(colorless_stroke, current_color);

		// Count error function if necessary:
		if (current_error_function < 0) {
			current_error_function = stroke_mse(image, stroke_points, current_color);
		}

		// Count error function for near points:
		auto shifted_color = current_color;
		// auto shifted_colored_stroke = this_colored_stroke;

		std::array<double, 3> gradient{};
		for (size_t color_dim_index = 0; color_dim_index < 3; ++color_dim_index) {
			auto& this_color_value = shifted_color[color_dim_index];
			this_color_value += little_color_delta;

			double new_error_function = stroke_mse(image, stroke_points, shifted_color);

			gradient[color_dim_index] = (new_error_function - current_error_function) / little_color_delta;

			this_color_value += little_color_delta;
		}

		return { gradient.begin(), gradient.end() }; // { gradient[0], gradient[1], gradient[2] };
	};

	auto error_function_counter =
		[&](const std::vector<double>& current_color_vector) -> double {
			color current_color(current_color_vector);
			return stroke_mse(image, stroke_points, current_color);
	};

	auto logging_callback = [](size_t iteration, double error_value) -> void {
		std::cout << "Iteration " << iteration << ": error = " << error_value << std::endl;
	};

	// TODO: Use GA? Use newton? Use gradient descent with checking each step if the value decreases
	// TODO: Have a combi-optimization built-in in GA library

	// Perform optimization: _______________________________________________---
	if (log_debug_information){
		std::cout << "____________________________________" << std::endl;
	}

	auto [optimization_mse, optimization_color_vector] = gradient_optimize(error_function_counter, error_function_gradient_counter,
			initial_color_component_sequence, learning_rate, iterations); // TODO: use logger if debugging enabled

	if (log_debug_information){
		std::cout << "____________________________________" << std::endl;
	}
	//______________________________________________________

	std::vector<double> best_color_vector;

	if (std::all_of(optimization_color_vector.begin(), optimization_color_vector.end(),
	                [](double value) -> bool {return not std::isnan(value); })
	    && not std::isnan(optimization_mse) && optimization_mse < initial_error) {

		best_color_vector = std::move(optimization_color_vector);
	}
	else {
		best_color_vector = std::move(initial_color_component_sequence);
	}

	auto best_color = color(best_color_vector);

	if (log_debug_information) {
		// Log the result:
		std::cout << "Optimization is finished!" << std::endl;
		std::cout << "Error for dummy color pick: " << initial_error << std::endl;
		std::cout << "Error for optimization color pick: " << optimization_mse << std::endl;
	}

	return best_color;
}


color find_stroke_color_by_ariphmetic_mean (const stroke &colorless_stroke, const Image &image)
{
	double r_sum = 0, g_sum = 0, b_sum = 0;
	size_t points_in_stroke = 0;

	colorless_stroke.for_each([&](size_t x, size_t y){
		const auto& image_pixel_color = image.at<cv::Vec3d>(y, x);

		r_sum += image_pixel_color[0];
		g_sum += image_pixel_color[1];
		b_sum += image_pixel_color[2];

		points_in_stroke++;
	}, 10000, get_image_range_limits(image));

	return color(
			r_sum / points_in_stroke,
			g_sum / points_in_stroke,
			b_sum / points_in_stroke
	);
}

color find_stroke_color (const stroke &colorless_stroke, const Image &image)
{
	return find_stroke_color_by_ariphmetic_mean(colorless_stroke, image);
}

void find_stroke_color (colored_stroke &colored_stroke, const Image &image)
{
	colored_stroke.background_color = find_stroke_color(reinterpret_cast<const stroke&>(colored_stroke), image);
}

void find_stroke_color (colored_stroke& colored_stroke, const color& setup_color)
{
    colored_stroke.background_color = setup_color;
}
