//
// Created by Vova on 22.07.2021.
//

#pragma once

#include <data_representation/range_rectangle.h>
#include "io_api/image_io_utils.h"
#include "PixelLayerTracker.h"


struct ColoredPosition
{
	size_t y, x;
	color c;
};


struct RasterizedPainting
{
	using PixelSet = std::vector<ColoredPosition>;

	Image image;
	Image layer_tracker;
	std::vector<std::vector<PixelLayerTracker>> layer_matrix;

	color m_canvas_color;

	explicit RasterizedPainting(size_t h, size_t w, color canvas_color);

	PixelSet get_pixel_list(const RangeRectangle<size_t>& bounding_box);

	void erase_pixels(const PixelSet& pixels);
	void copy_pixels_to_painting(const PixelSet& pixels);
	void add_pixel_layers(const PixelSet& pixels, size_t layer_index);
};


