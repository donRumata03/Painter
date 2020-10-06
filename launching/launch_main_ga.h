//
// Created by Vova on 02.10.2020.
//

#pragma once

#include <GA_operations/population_generator.h>
#include "painter_pch.h"

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "GA_operations/util.h"
#include "GA_operations/mutation.h"
#include "GA_operations/final_constrainer.h"
#include "GA_operations/final_fitness_function.h"
#include "GA_operations/stroke_range_generator.h"


#include "common_operations/basic_constraining.h"

class GA_launcher
{

public:
	explicit GA_launcher(std::string filename);

	GA_launcher(const GA_launcher&) = delete;
	GA_launcher(GA_launcher&&) = delete;
	GA_launcher operator= (const GA_launcher&) = delete;
	GA_launcher operator= (GA_launcher&&) = delete;

	~GA_launcher() = default;


	void configure_common_helpers();
	void configure_GA_operation_helpers();
	void run();
	void show_fitness_dynamic();

private:
	/// Constants:
	size_t stroke_number = 1000;
	size_t population_size = 10;
	size_t epoch_num = 10;

	double stroke_length_to_image_size_fraction = 0.05;
	double stroke_width_to_length_factor = 0.2; // Width = length * stroke_width_to_length_factor
	double stroke_coord_mutation_to_stroke_length_factor = 1.5;
	double stroke_width_mutation_to_stroke_width_factor = 0.4;

	/// For example, stroke sizes may vary from
	/// stroke_typical_length / sqrt(stroke_param_relative_range)
	///              to
	/// stroke_typical_length * sqrt(stroke_param_relative_range)
	double stroke_param_relative_range = 4;

	std::string filename;

	/// Run-time constants:
	size_t image_w = size_t(-1);
	size_t image_h = size_t(-1);

	double typical_coord = -1;
	double stroke_typical_length = -1;
	double stroke_typical_width = -1;
	double param_half_range = -1;

	double stroke_coord_mutation_sigma = -1;
	double stroke_width_mutation_sigma = -1;

	stroke_limit_descriptor limits{};

	/// Data:
	Image image;
	std::vector<double> GA_fitnesses;

	/// Common helpers:


	/// GA-specific data / actors:
	final_fitness_function* configured_fitness_function {};
	final_constrainer configured_constrainer{};
	final_generator configured_generator{};

	GA::GA_operation_set ga_operations;
	GA_params ga_params {};

	std::vector<std::pair<double, double>> point_ranges;
	std::vector<double> mutation_sigmas;
};


void launch_the_GA (const std::string &filename);