//
// Created by vova on 28.10.2020.
//


#pragma once

#include "common_operations/image_splitting_scheduling.h"
#include "common_operations/image_segregation.h"



inline void test_zone_scheduling() {
	for (auto& row : schedule_image_splitting(100, 100, 2, 2, 0.1).get_2d_cells()) {
		std::cout << row << std::endl;
	}
}

inline void test_image_segregation() {
	std::filesystem::path path_to_example_image = painter_base_path / "images_for_testing" / "van-gogh.jpg";

	Image image = open_image(path_to_example_image.string());

	auto splitting_plan = schedule_image_splitting(
			image.cols, image.rows, 5, 8, 0.1
			);

	for (auto& col : splitting_plan.get_2d_cells()) {
		std::cout << col << std::endl;
	}

	auto zones = split_image_into_zones(image, splitting_plan);

	auto segregation_output_path = painter_base_path / "resources" / "segregation_output";
	size_t x_index = 0;
	for (auto& zone_col : zones.images) {
		size_t y_index = 0;
		for (auto& zone : zone_col) {
			std::string filename = "x_index=" + std::to_string(x_index) + ",y_index=" + std::to_string(y_index) + ".png";
			std::cout << "Saving " << filename << std::endl;
			save_image(zone, segregation_output_path / filename);

			y_index++;
		}
		x_index++;
	}
}

