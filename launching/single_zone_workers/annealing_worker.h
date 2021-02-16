//
// Created by Vova on 16.02.2021.
//

#pragma once

#include <data_representation/color.h>
#include "painter_pch.h"
#include "single_zone_worker.h"



struct AnnealingImageParams {
	size_t iterations = 0;

	double typical_temperature = 1.;

	/// No multithreading: Single annealing optimization operation can't be performed multithreadingly!
};


class AnnealingWorker : public SingleZoneWorker
{
public:

	using ParametersType = AnnealingImageParams;

	AnnealingWorker (const Image& image, const AnnealingImageParams& params, const fs::path& logging_path);


	/// Run methods:
	void run_one_iteration () override;
	void run_remaining_iterations () override;

	/// Get the answer:
	const std::vector<double>& get_best_genome () override;


	/// Diagnostics:
	void show_fitness_dynamic () override;
	[[nodiscard]] double average_computation_time_seconds () const override;
	[[nodiscard]] double average_computation_time_per_pixel_seconds () const override;
	[[nodiscard]] double computations_performed () const override;

	void print_diagnostic_information () override;
};


