//
// Created by Vova on 28.10.2020.
//

#pragma once

#include "painter_pch.h"
#include "io_api/image_io_utils.h"

#include "common_operations/image_splitting_scheduling.h"


struct ImageZones
{
	ImageZoneRepresentation zone_descriptor;
	std::vector<std::vector<Image>> images;
};


ImageZones split_image_into_zones(const Image& image_to_crop, const ImageZoneRepresentation& zones);

