//
// Created by Vova on 09.03.2021.
//

#pragma once


struct ComputationalEfficiencyRepresentation
{
	size_t computations_performed = 0;
	size_t pixels_processed = 0;
	double time_spent_computing = 0;


	[[nodiscard]] double average_computational_time() const { return time_spent_computing / double(computations_performed); }
	[[nodiscard]] double computational_time_per_pixel() const { return time_spent_computing / double(pixels_processed); }
	[[nodiscard]] double pixels_per_second() const { return double(pixels_processed) / time_spent_computing; }

	void print_diagnostic_information() const;


	auto operator+ (const ComputationalEfficiencyRepresentation& other) const;
};


