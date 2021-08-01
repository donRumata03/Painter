//
// Created by Vova on 22.07.2021.
//

#pragma once

#include <data_representation/range_rectangle.h>
#include "io_api/image_io_utils.h"


struct RasterizedPainting
{
	using PixelSet = std::vector<std::pair<size_t, size_t>>;

	Image image;
	Image layer_tracker;
	std::vector<std::vector<size_t>> layer_matrix;

	color m_canvas_color;

	explicit RasterizedPainting(size_t h, size_t w, color canvas_color);

	PixelSet get_pixel_list(const RangeRectangle<size_t>& bounding_box);

	void erase_pixels(const PixelSet& pixels);
	void copy_pixels_to_painting(const PixelSet& pixels);
	void apply_layers(const PixelSet& pixels);
};


