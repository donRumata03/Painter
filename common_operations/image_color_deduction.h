//
// Created by Vova on 11.03.2021.
//

#pragma once

#include "io_api/image_io_utils.h"

color find_major_image_color (const Image& image, double minimal_allowed_percentage = 0.1);
color find_image_background_color (const Image& image, double max_can_be_non_background_color = 0.2);

