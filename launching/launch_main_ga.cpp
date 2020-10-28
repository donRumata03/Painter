//
// Created by Vova on 02.10.2020.
//

#include "launch_main_ga.h"

#include "GA_worker.h"

#include "GA_parameter_sets.h"

void launch_the_GA (const std::string &filename)
{
/*

	multizone_GA_launcher ga_launcher(filename);
	ga_launcher.configure_common_helpers();
	ga_launcher.configure_GA_operation_helpers();

	ga_launcher.run();
	ga_launcher.show_fitness_dynamic();

*/
	Image image = open_image(filename);

	GA_launching_params this_params = one_stroke_params;

	GA_worker worker(image, this_params);
	worker.run_remaining_iterations();

	/// For one thread:
	// Average computational time: 1.96496ms
	// Computational time per pixel: 13.0997 (ns/pix)
	// => Speed is 77 MegaPix / (sec * thread)

	/// For many threads without memory allocation saving system:
	// Average computational time: 8.70384ms
	// Computational time per pixel: 58.0256ns
	// => Computational speed: 17 MegaPixel / (sec * thread)

	/// For many threads with memory allocation saving system (measured WITH buffer getting):
	// 31 MegaPixel / (sec * thread)

	/// For many threads with memory allocation saving system (measured WITHOUT buffer getting):
	// 31 MegaPixel / (sec * thread)

	std::cout
		<< "Computations performed: " << worker.computations_performed() << "(" << this_params.epoch_num * this_params.population_size << " expected)" << std::endl
		<< "Average computational time: " << worker.average_computation_time_seconds() * 1e+3 << "ms" << std::endl
		<< "Computational time per pixel: " << worker.average_computation_time_per_pixel_seconds() * 1e+9 << "ns" << std::endl
		<< "=> Computational speed: " << size_t(std::round(1 / worker.average_computation_time_per_pixel_seconds() / 1e+6)) << " MegaPixel / (sec * thread)"
	<< std::endl;

	worker.show_fitness_dynamic();
}



multizone_GA_launcher::multizone_GA_launcher (Image image, size_t zones_x, size_t zones_y)
{

}
