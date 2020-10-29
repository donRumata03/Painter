//
// Created by Vova on 02.10.2020.
//

#include "launch_main_ga.h"

#include "GA_worker.h"

#include "GA_parameter_sets.h"

void launch_single_zone_GA (const std::string &filename)
{
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

void launch_multizone_GA (const std::string& filename)
{
	GA_launching_params this_params = one_stroke_params;
	image_splitting_params this_splitting_params = van_gogh_splitting_params;

	Image image = open_image(filename);
	multizone_GA_launcher launcher(image,
								this_splitting_params.zones_x, this_splitting_params.zones_y, this_splitting_params.overlay_percent,
								this_params);
	std::cout << "Performed initialization. Running.." << std::endl;

	launcher.run();
}


multizone_GA_launcher::multizone_GA_launcher (Image _image, size_t _zones_x, size_t _zones_y,
                                              double overlay_percent, GA_launching_params params)
							: zones(
									split_image_into_zones(
											_image,
											schedule_image_splitting(_image.cols, _image.rows, _zones_x, _zones_y, overlay_percent)
											)
										), zones_x(_zones_x), zones_y(_zones_y)
{
	image_w = this->image.cols;
	image_h = this->image.rows;

	image = std::move(_image);

	// std::cout << std::endl << zones.zone_descriptor << std::endl;

	/// Init workers:
	workers.reserve(zones_x);

	for (size_t worker_x_index = 0; worker_x_index < zones_x; ++worker_x_index) {
		auto& worker_col = workers.emplace_back();
		worker_col.reserve(zones_y);

		for (size_t worker_y_index = 0; worker_y_index < zones_y; ++worker_y_index) {
			std::string this_filename = "x_index=" + std::to_string(worker_x_index) + ",y_index=" + std::to_string(worker_y_index) + ".png";

			GA_worker w(zones.images[worker_x_index][worker_y_index], params, painter_base_path / "log" / "latest" / this_filename);

			worker_col.emplace_back(zones.images[worker_x_index][worker_y_index], params, painter_base_path / "log" / "latest" / this_filename);
		}
	}
}

void multizone_GA_launcher::run ()
{
	for(auto& col : workers) {
		for(auto& worker : col) {
			worker.run_remaining_iterations();
		}
	}
}





