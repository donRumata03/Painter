//
// Created by Vova on 16.02.2021.
//

#pragma once

#include <painter_pch.h>

#include <common_operations/image_segregation.h>

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "GA_operations/all.h"

#include "single_zone_workers/single_zone_worker.h"


/// Class that wraps dividing the image into a couple of zones:

template<class Worker>
class MultizoneLaunchWrapper
{

public:
	explicit MultizoneLaunchWrapper (Image _image, size_t _zones_x, size_t _zones_y,
	                                 double overlay_percent, typename Worker::ParametersType params);

	bool process_one_cell();
	void run();

	Image get_resultant_image();
	void save_result(const fs::path& filename);


	/// Memory managements stuff:
	MultizoneLaunchWrapper(const MultizoneLaunchWrapper&) = delete;
	MultizoneLaunchWrapper(MultizoneLaunchWrapper&&) = delete;
	MultizoneLaunchWrapper operator= (const MultizoneLaunchWrapper&) = delete;
	MultizoneLaunchWrapper operator= (MultizoneLaunchWrapper&&) = delete;
	~MultizoneLaunchWrapper() = default;


private:
	std::vector<double> glue_best_genomes();

private:
	ImageZones zones;
	std::vector<std::vector<std::shared_ptr<Worker>>> workers;
	std::vector<std::vector<bool>> workers_ready;

	Image image;

	image_logging_callback logger;
	size_t epochs_performed = 0;

	/// Run-time constants:
	size_t image_w = size_t(-1);
	size_t image_h = size_t(-1);

	size_t zones_x = size_t(-1);
	size_t zones_y = size_t(-1);
};


