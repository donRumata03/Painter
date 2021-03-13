//
// Created by Vova on 13.03.2021.
//

#pragma once

#include "common_operations/image_adaptive_params.h"


inline void test_zone_distributor(const fs::path& path)
{
	auto svg = SVG_service(path);
	svg.split_paths();

	ZoneResourceDistributor actor(svg, {});

	actor.visualize_resource_distribution();
}
