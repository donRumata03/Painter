#pragma once

#include <data_representation/color.h>
#include "painter_pch.h"

/// Dummy computing:

size_t calc_strokes_count (const cv::Mat& img, const cv::Size& size, size_t max_strokes, color canvas_color);

/// Smart computing (taking the «difficulty» of the region into account):

struct ZoneComplexityDescriptor
{
	double covered_area {};
	double stroke_perimeter {};
};


double estimate_stroke_complexity(ZoneComplexityDescriptor zone_descriptor);
std::vector<size_t> distribute_strokes_between_bezier_zones(const std::vector<ZoneComplexityDescriptor>& zone_descriptors, size_t total_strokes);