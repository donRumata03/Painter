//
// Created by Vova on 02.03.2021.
//

#pragma once

#include "painter_pch.h"

#include <common_operations/image_segregation.h>

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "GA_operations/all.h"

#include "launching/single_zone_workers/GA_worker.h"
#include "launching/single_zone_workers/annealing_worker.h"
#include "multizoner.h"

#include "vector_graphics/SVG_service.h"

#include <common_operations/filesystem_primitives.h>

/**
 * Is responsible for dividing the image into several zones
 * following the partitioning made by a pasteurizer such as Adobe Illustrator
 * and expressed in the SVG image given.
 *
 *
 * The ability of running optimizers for multiple zones in parallel is supported.
 *
 * For example, we have 10 worker threads for computing error function
 * and ≈ 10'000 optimization tasks (1 for each zone: to solve the Worker's task for the specified region)
 *
 * It means that it's more than reasonable to distribute the tasks between threads.
 */

template<class OptimizerType>
class SvgZoneLauncher
{
public:
	using OptimizerParameters = typename OptimizerType::ParametersType;


	SvgZoneLauncher(const fs::path& image_path, const CommonStrokingParams& stroking_params, const OptimizerParameters& custom_parameters,
				    bool parallelize = false, size_t worker_thread_number = std::thread::hardware_concurrency() - 2);

	void run();     /// TODO: parallelize this!

private:
	Image initial_image;

	CommonStrokingParams stroking_params;
	OptimizerParameters optimizer_parameters;

	static_thread_pool thread_pool;
	std::mutex common_worker_data_mutex;

	size_t zone_number = 0;
	std::vector<std::pair<size_t, size_t>> thread_zone_distribution;

	/// The common data being modified:
	SVG_service svg_manager;
	std::vector<OptimizerType> zone_optimizers;

	std::vector<colored_stroke> collected_strokes;
};

/// ________________________________________________________________________________________________________________________________________________
/// 															Implementations:
/// ________________________________________________________________________________________________________________________________________________




template <class OptimizerType>
SvgZoneLauncher<OptimizerType>::SvgZoneLauncher (const fs::path& image_path, const CommonStrokingParams& stroking_params,
                                                 const OptimizerParameters& custom_parameters, bool parallelize,
                                                 size_t worker_thread_number)
			: stroking_params(stroking_params), optimizer_parameters(custom_parameters)
{
	// Clear the old log:
	ensure_log_cleared();

	// Determine the number of zones and what the zones actually are
	svg_manager = SVG_service(image_path.string());
	svg_manager.split_paths();

	initial_image = svg_manager.get_raster_original_image();
	save_image(initial_image, (fs::path(painter_base_path) / "log" / "latest" / "original.png").string());

	/// Make task distribution:
	zone_number = svg_manager.get_shape_count();

	thread_pool.init(worker_thread_number, [this](size_t thread_index) {

	});
}


