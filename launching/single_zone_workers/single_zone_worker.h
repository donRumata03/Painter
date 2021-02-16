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


