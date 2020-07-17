//
// Created by Vova on 17.07.2020.
//

#pragma once


#include "optimization/stroke_color_optimizing.h"

inline void test_stroke_color_optimization_with_displaying() {
	std::string image_name = "brown_coffee.png";
	// std::string image_name = "blue_coffee.png";

	std::string input_path = (fs::path(base_path) / "resources" / "stroke_color_optimization_test" / "input" / image_name).string();

	Image image = open_image(input_path);

	stroke colorless_stroke {
			{1., 1.},
			{1., 1.},
			{1., 1.},
			1.
	}; // TODO : get appropriate coords

	auto col = find_stroke_color(colorless_stroke, image, 0.001, 1000, false);
}
