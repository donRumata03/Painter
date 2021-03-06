//
// Created by Vova on 11.03.2021.
//

#include "image_color_deduction.h"


/// As for now assume that that major color is the brightest
/// among colors, that cover >= image_size * p, p ∈ (0, 1) and are non-background (don't have ALL the corner pixels)

color find_major_image_color (const Image& image, double minimal_allowed_percentage)
{
	size_t image_area = image.cols * image.rows;
	auto minimal_allowed_pixels_for_zone = size_t(std::ceil(minimal_allowed_percentage * image_area));

	std::vector<color> color_list;

	cv::setNumThreads(0);
	image.forEach<Pixel>([&color_list](Pixel& pixel, const int position[]){
		auto this_col = color{pixel};

		color_list.push_back(this_col);

		//		auto this_brightness = this_col.brightness();
//		if (this_brightness > current_brightness) {
//			res = this_col;
//			current_brightness = this_brightness;
//		}
	});
	cv::setNumThreads(-1);

	auto pixel_color_distribution = make_Counter(color_list);

	std::vector<std::pair<color, size_t>> widely_spread_enough_pixel_color_distribution;
	std::copy_if(pixel_color_distribution.begin(), pixel_color_distribution.end(), std::back_inserter(widely_spread_enough_pixel_color_distribution),
			  [minimal_allowed_pixels_for_zone](const std::pair<color, size_t>& item){
		return item.second >= minimal_allowed_pixels_for_zone;
	});

	std::vector<std::pair<color, size_t>> non_background_color_data;
	std::copy_if(widely_spread_enough_pixel_color_distribution.begin(), widely_spread_enough_pixel_color_distribution.end(), std::back_inserter(non_background_color_data),
			  [&image](const std::pair<color, size_t> color_descriptor){
		/// The color is considered background only if all the 4 image's corners have that color
		auto col = color_descriptor.first;

		return
				col != color{ image.at<cv::Vec3d>(0, 0) } ||
				col != color{ image.at<cv::Vec3d>(image.rows - 1, 0 ) } ||
				col != color{ image.at<cv::Vec3d>(0, image.cols - 1) } ||
				col != color{ image.at<cv::Vec3d>(image.rows - 1, image.cols - 1) }
		;
	});


	if(non_background_color_data.empty()) {
		throw std::runtime_error("There are no colors satisfying requirements => can't find proper «major color»");
	}

	/// Sort by covered area (covered area is increasing):
	std::sort(non_background_color_data.begin(), non_background_color_data.end(), [](auto p1, auto p2){ return p1.second > p2.second; });


	/// Select the brightest of colors left:
	color res = { 0., 0., 0. };
	double current_brightness = 0;

	for (auto& col : non_background_color_data) {
		double this_brightness = col.first.brightness();

		if (this_brightness > current_brightness) {
			res = col.first;
			current_brightness = this_brightness;
		}
	}


	return res;
}

color find_image_background_color (const Image& image, double max_can_be_non_background_color)
{
	/// Goes though image's borders and look at their colors


	std::vector<color> colors_noticed;

	// Horizontal rows
	for (size_t x = 0; x < image.cols; ++x) {
		colors_noticed.emplace_back( image.at<cv::Vec3d>(0, x) );
	}
	for (size_t x = 0; x < image.cols; ++x) {
		colors_noticed.emplace_back( image.at<cv::Vec3d>(image.rows - 1, x) );
	}

	for (size_t y = 0; y < image.rows; ++y) {
		colors_noticed.emplace_back( image.at<cv::Vec3d>(y, 0) );
	}
	for (size_t y = 0; y < image.rows; ++y) {
		colors_noticed.emplace_back( image.at<cv::Vec3d>(y, image.cols - 1) );
	}

	auto distr = make_Counter(colors_noticed);

	color most_frequent_color{};
	size_t highest_frequency = 0;

	for (auto& this_col : distr) {
		if (this_col.second > highest_frequency) {
			most_frequent_color = this_col.first;
			highest_frequency = this_col.second;
		}
	}

	size_t total_pixels_viewed = colors_noticed.size();
	if (double(highest_frequency) >= total_pixels_viewed * (1 - max_can_be_non_background_color)) {
		std::cout << "[find_image_background_color]: Background's frequency: " << double(highest_frequency) / double(total_pixels_viewed) << std::endl;
		return most_frequent_color;
	}

	throw std::runtime_error("Can't find background color: no one is frequent enough!");
}
