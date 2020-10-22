//
// Created by Vova on 27.09.2020.
//

#pragma once

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "util.h"

struct RunTimeCounter {
	std::shared_ptr<std::atomic<size_t>> total_runs = nullptr;
	std::shared_ptr<std::atomic<double>> total_time_seconds = nullptr;
};

struct final_fitness_function
{
	Image initial_image {};

	size_t total_stroke_number {};
	size_t w{}, h{};

	bool is_run_sequentially {};
	mutable Image personal_buffer {};

	color canvas_color {};

	RunTimeCounter rt_counter;

	final_fitness_function() = default;

	/**
	 * @param is_run_sequentially: if it`s set to true, is stores only one buffer allocated in the constructor
	 * and uses it everywhere. This approach isn`t only applicable for sequential running!!
	 */
	explicit final_fitness_function(
			const Image& image,
			size_t strokes,
			bool is_run_sequentially,
			const color& canvas_color = { 0., 0., 0. }
			)
				: initial_image(image), total_stroke_number(strokes), is_run_sequentially(is_run_sequentially), canvas_color(canvas_color)
	{
		w = image.cols;
		h = image.rows;

		if (is_run_sequentially) {
			personal_buffer = make_default_image(w, h, canvas_color);
		}
	}

	double operator()(const std::vector<double>& stroke_data_buffer) const
	{
		assert(total_stroke_number * 7 == stroke_data_buffer.size()); // Should be dividable by 7 and the result should be equal to 7 * this->total_stroke_number (one double for each stuff)

		// Parse the stroke set and compute the colors for them:
/*
		std::vector<colored_stroke> strokes;

		for (size_t stroke_index = 0; stroke_index < total_stroke_number; ++stroke_index) {
			size_t stroke_value_initial_index = 7 * stroke_index;

			auto& unpacked_colored_stroke = strokes.emplace_back(
					stroke{
							{ stroke_data_buffer[stroke_value_initial_index], stroke_data_buffer[stroke_value_initial_index + 1] },
							{ stroke_data_buffer[stroke_value_initial_index + 2], stroke_data_buffer[stroke_value_initial_index +3] },
							{ stroke_data_buffer[stroke_value_initial_index + 4], stroke_data_buffer[stroke_value_initial_index +5] },
							stroke_data_buffer[stroke_value_initial_index + 6]
					},
					double {} // <- The stroke color is undefined at this moment
			);

			find_stroke_color(unpacked_colored_stroke, initial_image);
		}
*/
		Timer computation_timer;

		auto strokes = unpack_stroke_data_buffer(stroke_data_buffer);
		for (auto& stroke : strokes) find_stroke_color(stroke, initial_image);


		// Compute MSE; OpenCV reference counting system manages the memory properly:
		Image this_buffer = personal_buffer;

		if (!is_run_sequentially) {
			// Allocate the buffer:
			this_buffer = make_default_image(w, h, canvas_color);
		}

		// Rasterize strokes:
		rasterize_strokes(this_buffer, strokes);

		double MSE = image_mse(initial_image, this_buffer);


		auto this_canvas_color = canvas_color;
		if (is_run_sequentially) {
			// Clean-up the buffer:
			personal_buffer.forEach<Pixel>([this_canvas_color] (Pixel &pixel, const int position[]) {
				pixel.x = this_canvas_color.r;
				pixel.y = this_canvas_color.g;
				pixel.z = this_canvas_color.b;
			});
		}

		rt_counter.total_time_seconds->operator+=(computation_timer.get_time(Timer::time_units::seconds));
		rt_counter.total_runs->operator++();

		return 1 / MSE;
	}

};


