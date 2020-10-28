//
// Created by Vova on 02.10.2020.
//

#pragma once

#include "painter_pch.h"

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "GA_operations/all.h"


#include "GA/old_GA.h"

class multizone_GA_launcher
{

public:
	explicit multizone_GA_launcher(Image image, size_t zones_x, size_t zones_y);

	multizone_GA_launcher(const multizone_GA_launcher&) = delete;
	multizone_GA_launcher(multizone_GA_launcher&&) = delete;
	multizone_GA_launcher operator= (const multizone_GA_launcher&) = delete;
	multizone_GA_launcher operator= (multizone_GA_launcher&&) = delete;

	~multizone_GA_launcher() = default;


	void configure_common_helpers();
	void configure_GA_operation_helpers();
	void run();
	void show_fitness_dynamic();

private:
	std::vector<>

	/// Run-time constants:
	size_t image_w = size_t(-1);
	size_t image_h = size_t(-1);


	/// Data:
	Image image;
	std::vector<double> GA_fitnesses;

	/// Common helpers:


	/// GA-specific data / actors:
	final_fitness_function configured_fitness_function;
	final_constrainer configured_constrainer{};
	final_generator configured_generator{};
	final_crossover configured_crossover{};
	mutator configured_mutator{};

	image_logging_callback logger {};

	GA::GA_operation_set ga_operations;
	GA::continuous_GA_params ga_params {};

	std::vector<std::pair<double, double>> point_ranges;
	std::vector<double> mutation_sigmas;
};


void launch_the_GA (const std::string &filename);
