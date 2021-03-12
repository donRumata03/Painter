#include "image_adaptive_params.h"
#include "io_api/image_io_utils.h"



/// 																Dummy computing:

size_t calc_strokes_count (const cv::Mat& img, const cv::Size& size, size_t max_strokes, color canvas_color)
{
//    cv::Mat gray, thresh, u_img = convert_image_from_floating_point(img);
//    cv::cvtColor(u_img, gray, cv::COLOR_BGR2GRAY);
//    cv::threshold(gray, thresh, 0, 1, cv::THRESH_BINARY);

    size_t pixels = painted_pixel_number(img, canvas_color);
    return std::max(max_strokes * (pixels / (double)(size.width * size.height)), 1.);
}



/// 												ZoneResourceDistributor


ZoneResourceDistributor::ZoneResourceDistributor (const SVG_service& service,
                                                  color canvas_color,
                                                  const std::optional<std::function<double(const Image&, color)>>& complexity_estimator)
{
	std::optional<std::function<double(const Image&, color)>> optional_estimator = complexity_estimator;

	if (not optional_estimator) {
		optional_estimator = [](const Image& image, color canvas_color){ return double(painted_pixel_number(image, canvas_color)); };
	}
	auto real_estimator = *optional_estimator;

	auto temp_it = service.get_it();

	double total_sum = 0;
	std::vector<double> partial_sums(service.get_shapes_count());

	Image image_container;

	service.restart();
	for (auto& for_p_sum: partial_sums) {
		service.load_current_image(image_container);
		for_p_sum = real_estimator(image_container, canvas_color);

		service.next();
	}

	zone_fractions.resize(service.get_shapes_count());
	std::transform(partial_sums.begin(), partial_sums.end(), zone_fractions.begin(), [&total_sum](double partial_sum){ return partial_sum / total_sum; });

	service.set_iterator(temp_it);
}

std::vector<size_t> ZoneResourceDistributor::distribute_resource (size_t total_resource)
{
	std::vector<size_t> res(zone_fractions.size());
	std::transform(zone_fractions.begin(), zone_fractions.end(), res.begin(), [&total_resource](double this_zone_fraction){
		return size_t(std::round(this_zone_fraction * double(total_resource)));
	});

	return res;
}



/// Non-dummy functionality:


/**
 * \note: if the zones are pretty ragged, this might be not an ideal estimator and distributor
 */
double estimate_stroke_complexity (ZoneComplexityDescriptor zone_descriptor)
{
	// TODO: test me ! ;(

	/// HyperParameters:
	constexpr double max_perimeter_contribution = 1.3; /// Is essentially an asymptote of the relative perimeter contribution

	constexpr double characteristic_y = 0.9;
	constexpr double x_of_characteristic_y = 5.;
	double k = -x_of_characteristic_y / std::log(1 - characteristic_y);
	std::cout << "[Estimating Stroke Complexity]: k is " << k << std::endl;


	/// Considering circle the easiest figure of the given area

	double circles_length = 2 * std::sqrt(pi * zone_descriptor.covered_area); // Length of a circle of the same area
	if (zone_descriptor.stroke_perimeter > circles_length + 1e-7) {
		throw std::range_error("Perimeter can't be so small!");
	}

	double relative_excess = (zone_descriptor.stroke_perimeter - circles_length) / circles_length;
	assert(relative_excess >= -1e-10);
	// If it's 10 times bigger than the circle's one, it isn't a big difference between it and 9-time-bigger-zone
	// But it isn't true for 2 and 3 difference

	double perimeter_contribution = max_perimeter_contribution * (1 - std::exp(-relative_excess / k));

	double result = zone_descriptor.covered_area * (1 + perimeter_contribution);

	return result;
}



std::vector<size_t> distribute_strokes_between_bezier_zones (const std::vector<ZoneComplexityDescriptor>& zone_descriptors, size_t total_strokes)
{
	std::vector<double> complexities(zone_descriptors.size());
	std::transform(zone_descriptors.begin(), zone_descriptors.end(), complexities.begin(), [](auto zone) { return estimate_stroke_complexity(zone); });

	double overall_complexity = std::accumulate(complexities.begin(), complexities.end(), 0.);

	std::vector<size_t> res;
	res.reserve(zone_descriptors.size());
	std::transform(complexities.begin(), complexities.end(), res.begin(), [&total_strokes, &overall_complexity](double local_complexity) -> size_t {
		return size_t(std::round( double(total_strokes) * local_complexity / overall_complexity ));
	});

	return res;
}

