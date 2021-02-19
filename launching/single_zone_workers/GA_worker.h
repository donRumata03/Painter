//
// Created by Vova on 22.10.2020.
//

#pragma once

#include "painter_pch.h"

#include "common_operations/stroke_limits.h"
#include "io_api/image_io_utils.h"
#include "rasterization/stroke_rasterizer.h"

#include "GA_operations/all.h"
#include "single_zone_worker.h"


struct GA_launching_params : public CommonStrokingParams
{
	/// Main params (computation time is almost proportional to their «product»):
	size_t stroke_number = 0;
	size_t population_size = 0;
	size_t epoch_num = 0;



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



class GA_worker : public SingleZoneWorker
{
public:
	using ParametersType = GA_launching_params;

	GA_worker (const Image& image, const GA_launching_params& params, const fs::path& logging_path = fs::path{ painter_base_path } / "log" / "latest");

	void run_one_iteration() override;
	void run_remaining_iterations() override;

	void show_fitness_dynamic () override;
	double average_computation_time_seconds() const override { return configured_fitness_function.average_computation_time_seconds(); }
	double average_computation_time_per_pixel_seconds() const override { return configured_fitness_function.average_computation_time_per_pixel_seconds(); }
	double computations_performed() const override { return configured_fitness_function.computations_performed(); }

	const std::vector<double>& get_best_genome() override;

	void print_diagnostic_information() override {
		std::cout
				<< "Computations performed: " << computations_performed() << " (" << launch_params.epoch_num * launch_params.population_size << " expected)" << std::endl
				<< "Average computational time: " << average_computation_time_seconds() * 1e+3 << "ms" << std::endl
				<< "Computational time per pixel: " << average_computation_time_per_pixel_seconds() * 1e+9 << "ns" << std::endl
				<< "=> Computational speed: " << size_t(std::round(1 / average_computation_time_per_pixel_seconds() / 1e+6)) << " MegaPixel / (sec * thread)"
				<< std::endl;
	}


private:
	// Image
	Image initial_image;
	size_t image_w;
	size_t image_h;


	GA_launching_params launch_params;


	/// Helpers:
	stroke_limit_descriptor limits{};

	/// GA operations:
	final_fitness_function  configured_fitness_function;
	final_constrainer       configured_constrainer{};
	final_population_generator         configured_generator{};
	final_crossover         configured_crossover{};
	mutator                 configured_mutator{};

	image_logging_callback logger {};

	/// GA init data:
	GA::GA_operation_set ga_operations;
	GA::continuous_GA_params ga_params {};

	std::vector<std::pair<double, double>> point_ranges;
	std::vector<double> mutation_sigmas;

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

