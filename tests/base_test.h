//
// Created by Vova on 26.07.2020.
//

#pragma once

#include "rasterization/drawer.h"

inline Image get_default_image() {
	size_t image_width = 1920;
	size_t image_height = 1080;

	Image image(image_height, image_width, CV_64FC3, {0., 0., 0.});

	return image;
}
