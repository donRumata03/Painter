//
// Created by Vova on 05.10.2020.
//

#pragma once

#include <io_api/image_io_utils.h>
#include <rasterization/stroke_rasterizer.h>

#include "painter_pch.h"

#include "GA_operations/util.h"

class GA_informer
{
	const Image& initial_image;
	size_t total_iterations = 0;

public:
	explicit GA_informer(const Image& image, size_t _total_iterations) : initial_image(image), total_iterations(_total_iterations) {}

	void operator() (size_t GA_iterations, double best_fitness_value, const std::vector<double>& best_stroke_genome){
		std::cout << "GA percent: \t" << percent_plotter(GA_iterations, total_iterations, 1) << "; Best MSE of error function is: \t" << 1 / best_fitness_value << std::endl;

		// std::cout << best_stroke_genome << std::endl;

		return;

		// Show image:
		auto strokes = unpack_stroke_data_buffer(best_stroke_genome);
		colorize_strokes(strokes, initial_image);

		Image new_image = make_default_image(initial_image.cols, initial_image.rows);
		rasterize_strokes(new_image, strokes);

		show_image_in_system_viewer(new_image);

		// std::cout << "Closed explorer?" << std::endl; // <- YES!
	}
};


