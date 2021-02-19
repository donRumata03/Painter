//
// Created by Vova on 16.02.2021.
//

#pragma once

#include "painter_pch.h"


class SingleZoneWorker
{
public:

	/// Run:
	virtual void run_one_iteration() = 0;
	virtual void run_remaining_iterations() = 0;

	/// Get output:
	virtual const std::vector<double>& get_best_genome() = 0;

	/// Diagnostic:
	virtual void show_fitness_dynamic () = 0;
	[[nodiscard]] virtual double average_computation_time_seconds() const = 0;
	[[nodiscard]] virtual double average_computation_time_per_pixel_seconds() const = 0;
	[[nodiscard]] virtual double computations_performed() const = 0;
	virtual void print_diagnostic_information() = 0;
};


struct CommonStrokingParams
{
	size_t stroke_number = 0;
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
};
