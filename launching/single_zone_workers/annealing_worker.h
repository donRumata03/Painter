//
// Created by Vova on 16.02.2021.
//

#pragma once

#include <data_representation/color.h>

#include <GA_operations/all.h>

#include <Annealing/annealing_optimize.h>
#include <GA_operations/callbacks/on_improvement_updating_logger.h>
#include "painter_pch.h"
#include "single_zone_worker.h"



struct AnnealingStrokingParams {
	size_t iterations = 0;

	double typical_temperature = 1.;

	double gene_mutation_fraction = 0.1;

	/// No multithreading: Single annealing optimization operation can't be performed multithreadingly!

	[[nodiscard]] size_t computations_expected() const { return iterations; }
};


class AnnealingWorker : public SingleZoneWorker
{
public:

	using ParametersType = AnnealingStrokingParams;

	AnnealingWorker (const Image& image, const CommonStrokingParams& stroking_params, const AnnealingStrokingParams& annealing_params,
                    const fs::path& logging_path = fs::path{ painter_base_path } / "log" / "latest", bool enable_console_output = true);


	/// Run methods:
	void run_one_iteration () override;
	void run_remaining_iterations () override;

	/// Get the answer:
	const std::vector<double>& get_best_genome () override;


	/// Diagnostics:
	ComputationalEfficiencyRepresentation get_efficiency_account() const override { return configured_error_function.get_efficiency_account(); }

//	[[nodiscard]] double average_computation_time_seconds () const override;
//	[[nodiscard]] double average_computation_time_per_pixel_seconds () const override;
//	[[nodiscard]] double computations_performed () const override;
//	double time_spent_counting () const override;

	void show_fitness_dynamic () override;
	// void print_diagnostic_information () override;


private:
	/// Image
	Image initial_image;
	size_t image_w;
	size_t image_h;

	/// Params:
	CommonStrokingParams common_stroking_params;
	AnnealingStrokingParams annealing_stroking_params;

	/// Actors:
	final_fitness_function configured_error_function;
	AdjustableGenomeMutator configured_mutator;
	FinalGenomeGenerator configured_generator;
	OnImprovementUpdatingLogger logger;


	/// Results:
	AnnealingOptimizationOutput<double> result;
	// std::vector<double> best_genome;

	/// Diagnostics:
	// std::vector<double> target_function_dynamic;



};


