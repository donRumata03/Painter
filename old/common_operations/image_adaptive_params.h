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
public:
	// TODO: test me!

	ZoneResourceDistributor(const SVG_service& service,
	                        color canvas_color,
						    const std::optional<std::function<double(const Image&, color)>>& complexity_estimator = std::nullopt);

	std::vector<size_t> distribute_resource (size_t total_resource, size_t minimal_allowed_param = 0);



	/// Statistics for image analyzing:
	void add_resource_distribution_to_plot();
	void add_distributed_resource_to_plot (size_t total_resource, size_t minimal_allowed_param);
	void show_plot_with_parameters();

		/// Inequality measures:
	void print_inequality_account();

			/// Different Metrics:
	double compute_Gini_index();
	double top_1_percent_wealth_share();
	double having_half_wealth_share_fraction();

			/// Graphs:
	void add_cumulative_inequality_graph_to_plot();



private:
	std::vector<double> get_sorted_fractions (bool from_lowest = false);
	std::vector<std::pair<double, double>> get_cumulative_inequality_graph();

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