//
// Created by vova on 04.08.2021.
//

#include "PixelLayerTracker.h"


bool PixelColorByPriority::operator< (const PixelColorByPriority& rhs) const
{
	return priority < rhs.priority;
}

bool PixelColorByPriority::operator> (const PixelColorByPriority& rhs) const
{
	return rhs < *this;
}

bool PixelColorByPriority::operator<= (const PixelColorByPriority& rhs) const
{
	return !(rhs < *this);
}

bool PixelColorByPriority::operator>= (const PixelColorByPriority& rhs) const
{
	return !(*this < rhs);
}


///												Pixel Layer Tracker

color PixelLayerTracker::get_pixel_color()
{
	// TODO: add color blending!

	return m_data.end()->second;
}

void PixelLayerTracker::add_color_layer (li priority, color c)
{
	m_data[priority] = c;
}

color PixelLayerTracker::remove_layer_by_priority (li priority)
{
	auto it = m_data.find(priority);
	color c = it->second;
	m_data.erase(it);

	return c;
}
