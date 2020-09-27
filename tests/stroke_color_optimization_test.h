//
// Created by Vova on 17.07.2020.
//

#pragma once


#include "rasterization/stroke_rasterizer.h"
#include "optimization/stroke_color_optimizing.h"

inline void test_stroke_color_optimization_with_displaying() {
	std::string image_name = "brown_coffee.png";
	// std::string image_name = "blue_coffee.png";

	std::string input_path = (fs::path(base_path) / "resources" / "stroke_color_optimization_test" / "input" / image_name).string();

	Image image = open_image(input_path);

	std::vector<std::pair<double, double>> points = {
			{ 295, 100 }, { 276, 195 }, { 410, 244 }
	}; // TODO : get appropriate coords

	stroke colorless_stroke {
			points[0], points[1], points[2], 30
	};

	// auto optimal_color = find_stroke_color_by_gradient_descent(colorless_stroke, image, 0.001, 1000, false);
	auto optimal_color = find_stroke_color_by_ariphmetic_mean(colorless_stroke, image);

	std::cout << "Optimal color is " << optimal_color << std::endl;

	// Display the stroke:
	rasterize_stroke(image, colored_stroke(colorless_stroke, optimal_color));

	auto out_path = (fs::path(base_path) / ("resources/stroke_color_optimization_test/output"s + image_name)).string();

	// save_image(image, out_path);
	// system(out_path); // -> Show image in system viewer
	show_image_in_system_viewer(image, "fitted_stroke_color");
}
