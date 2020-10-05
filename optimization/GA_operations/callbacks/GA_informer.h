//
// Created by Vova on 05.10.2020.
//

#pragma once
#include <utility>
#include <io_api/image_io_utils.h>
#include <rasterization/stroke_rasterizer.h>

#include "painter_pch.h"

#include "optimization/GA_operations/util.h"

class GA_informer
{
	Image initial_image;

public:
	explicit GA_informer(Image image) : initial_image(std::move(image)) {}

	void operator() (double GA_percent, double best_fitness_value, const std::vector<double>& best_stroke_genome){
		std::cout << "GA percent is: \t" << GA_percent << "; Best MSE of error function is: \t" << 1 / best_fitness_value << std::endl;

		// Show image:
		auto strokes = unpack_stroke_data_buffer(best_stroke_genome);
		colorize_strokes(strokes, initial_image);

		Image new_image = make_default_image(initial_image.cols, initial_image.rows);
		rasterize_strokes(new_image, strokes);

		show_image_in_system_viewer(new_image);
	}
};


