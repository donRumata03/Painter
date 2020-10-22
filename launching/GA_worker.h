//
// Created by Vova on 22.10.2020.
//

#pragma once

#include "painter_pch.h"

#include "common_operations/stroke_limits.h"
#include "io_api/image_io_utils.h"
#include "rasterization/stroke_rasterizer.h"

#include "GA_operations/util.h"


struct GA_launching_params
{
	/// Main params (computation time is almost proportional to their «product»):
	size_t stroke_number;
	size_t population_size;
	size_t epoch_num;



	/// Fractions:
	double stroke_length_to_image_size_fraction = 0.15; // 0.05;
	double stroke_width_to_length_factor = 0.5; // 0.4; // Width = length * stroke_width_to_length_factor
	double stroke_coord_mutation_to_stroke_length_factor = 0.5;
	double stroke_width_mutation_to_stroke_width_factor = 0.5; // 0.1;

	double stroke_param_relative_range = 3;

	double move_mutation_probability = 0.2;
	double logging_percentage = 0.00;

	///
	color canvas_color = { 0., 0., 0. };

	bool allow_multithreading = false;

};



class GA_worker
{
public:

	GA_worker(const Image& image, const GA_launching_params& params);

	void run_one_iteration();
	void run_remaining_iterations();

private:
	// Image
	Image initial_image;
	size_t image_w;
	size_t image_h;


	GA_launching_params launch_params;


	/// Helpers:
	stroke_limit_descriptor limits{};

	/// GA operations:


	/// Optimizer itself:
	std::optional<GA::GA_optimizer> optimizer;

	/// Length tuning:
	double typical_coord;
	double stroke_typical_length;
	double stroke_typical_width;
	double param_half_range;

	double stroke_coord_mutation_sigma;
	double stroke_width_mutation_sigma;

};


