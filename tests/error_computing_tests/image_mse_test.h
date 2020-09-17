//
// Created by Vova on 27.08.2020.
//

#pragma once

#include "optimization/error_computing.h"


inline void test_image_mse()
{
	std::filesystem::path input_path = fs::path(base_path) / "resources" / "image_mse_test";

	// Open the images:
	Image original_image = open_image((input_path / "brown_coffee.png").string());
	Image faraway_image = open_image((input_path / "blue_coffee.png").string());
	Image middle_image = open_image((input_path / "almost_brown_coffee.png").string());

	//
	std::cout << "Original vs. Faraway: " << image_mse(original_image, faraway_image, false) << std::endl;
	std::cout << "Original vs. Middle: " << image_mse(original_image, middle_image, false) << std::endl;
	std::cout << "Faraway vs. Middle: " << image_mse(middle_image, faraway_image, false) << std::endl;
}


inline void test_parallel_image_mse()
{

}
