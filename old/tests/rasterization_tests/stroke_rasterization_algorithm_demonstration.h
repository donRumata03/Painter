//
// Created by Vova on 16.07.2020.
//

#pragma once

#include "rasterization/stroke_rasterizer.h"
#include "tests/test_base.h"

inline void compare_stroke_rasterizaton_algos(){
/*
	size_t image_width = 1920;
	size_t image_height = 1080;

	Image image(image_height, image_width, CV_64FC3, {0., 0., 0.});
*/

	std::vector<std::pair<double, double>> points = {
			{ 110, 110 }, { 200, 500 }, { 1000, 800 }
	};


	for (auto algo : { StrokeRasterizationAlgorithm::vertical_lines, StrokeRasterizationAlgorithm::smooth }) {
		auto image = get_default_image(); // 1920 × 1080

		rasterize_stroke(image,
		                 colored_stroke{ points[0], points[1], points[2],
		                                 100, {1., 1., 1}}, 10000, algo);

		// Draw the points:
		double r = 10;
		for (auto& point : points) {
			auto c = circle { point, r };
			auto col = color{1., 0.5, 0.5};
			draw_circle(image, c, col);
		}

		show_image_in_system_viewer(image);
	}
}

