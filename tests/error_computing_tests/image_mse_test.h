//
// Created by Vova on 27.08.2020.
//

#pragma once

#include "optimization/error/error_computing.h"


inline void test_image_mse()
{
	std::filesystem::path input_path = fs::path(base_path) / "resources" / "image_mse_test";


	// Open the images:
	Image original_image = open_image((input_path / "brown_coffee.png").string());
	Image faraway_image = open_image((input_path / "blue_coffee.png").string());
	Image middle_image = open_image((input_path / "almost_brown_coffee.png").string());

	std::cout << original_image.size << "\n";
	std::cout << faraway_image.size << "\n";
	std::cout << middle_image.size << "\n";

	std::cout << console_colors::green << "Loaded images!" << console_colors::remove_all_colors << std::endl;

	//
	std::cout << "Original vs. Faraway: \t" << image_mse(original_image, faraway_image, false) << std::endl;
	std::cout << "Original vs. Middle: \t" << image_mse(original_image, middle_image, false) << std::endl;
	std::cout << "Faraway vs. Middle: \t" << image_mse(middle_image, faraway_image, false) << std::endl;
}


inline void test_parallel_image_mse()
{
	// Comparing Original vs. Faraway: with standard and with parallel algorithms:
	std::filesystem::path input_path = fs::path(base_path) / "resources" / "image_mse_test";


	Image original_image = open_image((input_path / "brown_coffee.png").string());
	Image faraway_image = open_image((input_path / "blue_coffee.png").string());

	std::cout << console_colors::green << "Loaded images!" << console_colors::remove_all_colors << std::endl;

	std::cout << "Standard: " << image_mse(original_image, faraway_image, false) << std::endl;
	std::cout << "Parallel: " << image_mse(original_image, faraway_image, true) << std::endl;
}

inline void compare_image_mse_performance_parallel() {
	// Comparing Original vs. Faraway: with standard and with parallel algorithms:
	size_t times_of_execution = 30;

	std::filesystem::path input_path = fs::path(base_path) / "resources" / "image_mse_test";


	Image original_image = open_image((input_path / "brown_coffee.png").string());
	Image faraway_image = open_image((input_path / "blue_coffee.png").string());

	std::cout << console_colors::green << "Loaded images!" << console_colors::remove_all_colors << std::endl;

	std::cout << "\t\t\t\tPerformance tests:" << std::endl;

	std::vector<double> sequential_time_measurements, parallel_time_measurements;

	std::cout << measure_function_execution_time([&original_image, &faraway_image](){
		double res = image_mse(original_image, faraway_image, false);
		}, times_of_execution, "Standard MSE", &sequential_time_measurements) << " ms" << std::endl;

	std::cout << measure_function_execution_time([&original_image, &faraway_image](){
		double res = image_mse(original_image, faraway_image, true);
	}, times_of_execution, "Parallel MSE", &parallel_time_measurements) << " ms" << std::endl;

	std::cout << "Standard time measurements: " << sequential_time_measurements << std::endl;
	std::cout << "Parallel time measurements: " << parallel_time_measurements << std::endl;

	auto sequential_distribution_graph = count_number_density(sequential_time_measurements, 0.4, times_of_execution * 10);
	auto parallel_distribution_graph = count_number_density(parallel_time_measurements, 0.4, times_of_execution * 10);


	std::cout << "Standard time distribution: " << sequential_distribution_graph << std::endl;
	std::cout << "Parallel time distribution: " << parallel_distribution_graph << std::endl;


	add_to_plot(sequential_distribution_graph, {
		.name = "Time Without parallel"
	});
	add_to_plot(parallel_distribution_graph, {
		.name = "Parallel"
	});

	show_plot({
		.window_title = "Sequential vs. Parallel"
	});
}


