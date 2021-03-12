#pragma once

#include "painter_pch.h"

#include <data_representation/color.h>
#include "figure_area_and_perimeter.h"

#include <vector_graphics/SVG_service.h>

/// Dummy computing:

size_t calc_strokes_count (const cv::Mat& img, const cv::Size& size, size_t max_strokes, color canvas_color);


/// Computing, taking all the zones into account:

class ZoneResourceDistributor
{
	// TODO: test me!

	ZoneResourceDistributor(const SVG_service& service,
	                        color canvas_color,
						    const std::optional<std::function<double(const Image&, color)>>& complexity_estimator = std::nullopt);

	std::vector<size_t> distribute_resource(size_t total_resource);

private:
	std::vector<double> zone_fractions;
};


/// Smart computing (taking the «difficulty» of the region into account):

struct ZoneComplexityDescriptor
{
	double covered_area {};
	double stroke_perimeter {};
};


double estimate_stroke_complexity(ZoneComplexityDescriptor zone_descriptor);
std::vector<size_t> distribute_strokes_between_bezier_zones(const std::vector<ZoneComplexityDescriptor>& zone_descriptors, size_t total_strokes);