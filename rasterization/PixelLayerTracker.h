//
// Created by vova on 04.08.2021.
//

#pragma once

#include "io_api/image_io_utils.h"


struct PixelColorByPriority
{
	size_t priority = 0;
	color c{};

	bool operator< (const PixelColorByPriority& rhs) const;
	bool operator> (const PixelColorByPriority& rhs) const;
	bool operator<= (const PixelColorByPriority& rhs) const;
	bool operator>= (const PixelColorByPriority& rhs) const;
};


/**
 * Stores color layers with priority for pixels
 */
class PixelLayerTracker
{
private:
	std::map<li, color> m_data;

public:
	void add_color_layer(li priority, color c);
	color remove_layer_by_priority(li priority);
	color get_pixel_color();
};
