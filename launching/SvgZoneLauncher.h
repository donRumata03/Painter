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


/**
 * Is responsible for dividing the image into several zones
 * following the partitioning made by a pasteurizer such as Adobe Illustrator
 * and expressed in the SVG image given.
 *
 *
 * The ability of running optimizers for multiple zones in parallel is supported.
 */

template<class OptimizerType>
class SvgZoneLauncher                   /// TODO!
{
public:
	using OptimizerParameters = typename OptimizerType::ParametersType;


	SvgZoneLauncher(Image image, const CommonStrokingParams& stroking_params, const OptimizerParameters& custom_parameters,
				    bool parallelize = false, size_t worker_thread_number = std::thread::hardware_concurrency() - 2);

	void run();

private:

	SVG_service svg_manager;

	static_thread_pool thread_pool;
};


