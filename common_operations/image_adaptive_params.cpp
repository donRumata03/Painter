#include "image_adaptive_params.h"
#include "io_api/image_io_utils.h"


size_t calc_strokes_count(const cv::Mat& img, const cv::Size& size, const size_t max_strokes)
{
    cv::Mat gray, thresh, u_img = convert_image_from_floating_point(img);
    cv::cvtColor(u_img, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thresh, 0, 1, cv::THRESH_BINARY);

    size_t pixels = cv::sum(thresh)[0];
    return std::max(max_strokes * (pixels / (double)(size.width * size.height)), 1.);
}


/// Non-dummy functionality:


/**
 * \note: if the zones are pretty ragged, this might be not an ideal estimator and distributor
 */
double estimate_stroke_complexity (StrokeComplexityDescriptor stroke_descriptor)
{
	/// HyperParameters:
	constexpr double max_perimeter_contribution = 1.75; /// Is essentially an asymptote of the relative perimeter contribution

	constexpr double characteristic_y = 0.9;
	constexpr double x_of_characteristic_y = 5.;
	double k = -x_of_characteristic_y / std::log(1 - characteristic_y);
	std::cout << "[Estimating Stroke Complexity]: k is " << k << std::endl;


	/// TODO
	/// Considering circle the easiest figure of the given area

	double circles_length = 2 * std::sqrt(pi * stroke_descriptor.covered_area); // Length of a circle of the same area
	if (stroke_descriptor.stroke_perimeter > circles_length + 1e-7) {
		throw std::range_error("Perimeter can't be so small!");
	}

	double relative_excess = (stroke_descriptor.stroke_perimeter - circles_length) / circles_length;
	// If it's 10 times bigger than the circle's one, it isn't a big difference between it and 9-time-bigger-zone
	// But it isn't true for 2 and 3 difference

	double perimeter_contribution = max_perimeter_contribution * (1 - std::exp(-relative_excess / k));

	double result = stroke_descriptor.covered_area * (1 + perimeter_contribution);

	return result;
}
