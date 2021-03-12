//
// Created by Vova on 09.03.2021.
//

#include "computational_efficiency_representation.h"

ComputationalEfficiencyRepresentation ComputationalEfficiencyRepresentation::operator+ (const ComputationalEfficiencyRepresentation& other) const
{
	return ComputationalEfficiencyRepresentation {
			computations_performed + other.computations_performed,
			pixels_processed + other.pixels_processed,
			time_spent_computing + other.time_spent_computing,
	};
}

void ComputationalEfficiencyRepresentation::print_diagnostic_information () const
{
	size_t exp_precision = 10;

	std::cout
			<< "Computations performed: " << computations_performed << std::endl
			<< "Average computational time: " << average_computational_time() * 1e+3 << "ms" << std::endl
			<< "Computational time per pixel: " << computational_time_per_pixel() * 1e+9 << "ns" << std::endl
			<< "=> Computational speed: " << std::round(pixels_per_second() / 1e+6 * exp_precision) / exp_precision << " MegaPixel / sec"
			<< std::endl;

}
