//
// Created by Vova on 02.10.2020.
//

#include "launch_main_ga.h"

#include "launching/single_zone_workers/GA_worker.h"

#include "GA_parameter_sets.h"

void launch_single_zone_GA (const std::string &filename)
{
	Image image = open_image(filename);

	GA_launching_params this_params = van_gogh_params;

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

	worker.print_diagnostic_information();
	worker.show_fitness_dynamic();
}

void launch_multizone_GA (const std::string& filename)
{
	GA_launching_params this_params = van_gogh_params;
	image_splitting_params this_splitting_params = van_gogh_splitting_params;

	Image image = open_image(filename);
	MultizoneLaunchWrapper<GA_worker> launcher(image,
	                                this_splitting_params.zones_x, this_splitting_params.zones_y, this_splitting_params.overlay_percent,
	                                this_params);
	std::cout << "[main launching function]: Performed initialization. Running.." << std::endl;

	launcher.run();

	launcher.save_result(painter_base_path / "log" / "latest" / "result.png");
}





