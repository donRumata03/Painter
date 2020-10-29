//
// Created by Vova on 02.10.2020.
//

#pragma once

#include <common_operations/image_segregation.h>
#include "painter_pch.h"

#include "optimization/error/error_computing.h"
#include "optimization/stroke_color_optimizing.h"

#include "GA_operations/all.h"

#include "GA_worker.h"

class multizone_GA_launcher
{

public:
	explicit multizone_GA_launcher (Image _image, size_t _zones_x, size_t _zones_y, double overlay_percent);

	multizone_GA_launcher(const multizone_GA_launcher&) = delete;
	multizone_GA_launcher(multizone_GA_launcher&&) = delete;
	multizone_GA_launcher operator= (const multizone_GA_launcher&) = delete;
	multizone_GA_launcher operator= (multizone_GA_launcher&&) = delete;

	~multizone_GA_launcher() = default;

private:
	ImageZones zones;
	std::vector<std::vector<GA_worker>> workers;



	/// Run-time constants:
	size_t image_w = size_t(-1);
	size_t image_h = size_t(-1);

	size_t zones_x = size_t(-1);
	size_t zones_y = size_t(-1);

	/// Data:
	Image image;

};


void launch_single_zone_GA (const std::string &filename);
void launch_multizone_GA (const std::string &filename);
