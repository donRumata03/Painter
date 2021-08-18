//
// Created by Vova on 12.03.2021.
//

#pragma once

#include "common_operations/figure_area_and_perimeter.h"


inline void test_painted_area_counting() {
	fs::path example_figure_path = painter_base_path / "images_for_testing" / "zone_example.png";
	Image image = open_image(example_figure_path.string());

	std::cout << "Box area: " << image.rows * image.cols << std::endl;
	std::cout << "OpenCV's method: " << painted_pixel_number_black_canvas(image) << std::endl;
	std::cout << "Simple method: " << painted_pixel_number(image, { 0., 0., 0. }) << std::endl;
}


inline void test_painted_figures_perimeters_counting() {
	// TODO
}
