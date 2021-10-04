#pragma once

#include "operations/image/splitting.h"
#include "utils/image.h"
#include "painter_pch.h"

struct ImageZones {
  ImageZoneRepresentation zone_descriptor;
  std::vector<std::vector<Image>> images;
};

ImageZones split_image_into_zones(const Image& image_to_crop, const ImageZoneRepresentation& zones);
