//
// Created by Vova on 22.07.2021.
//

#pragma once

#include "io_api/image_io_utils.h"


struct RasterizedPainting
{
	Image image;
	std::vector<std::vector<size_t>> layer_matrix;

	RasterizedPainting(Image external_image) {
		image = external_image;
		layer_matrix = std::vector<std::vector<size_t>>(
				image.rows, std::vector<size_t>(image.cols, size_t(0))
				);
	}
};


