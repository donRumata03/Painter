//
// Created by Vova on 16.07.2020.
//

#include "error_computing.h"

double image_mse (const Image &image1, const Image &image2)
{
	assert(image1.size == image2.size());
	size_t w = image1.cols, h = image1.rows;
	auto area = double(w * h);

	double diff_sum = 0;
	for (size_t x = 0; x < w; ++x) {
		for (size_t y = 0; y < h; ++y) {
			const auto& first_pixel = image1.at<cv::Vec3d>(y, x);
			const auto& second_pixel = image2.at<cv::Vec3d>(y, x);

			/*
			double red_diff = square( first_pixel[0] - second_pixel[0] );
			double green_diff = square( first_pixel[1] - second_pixel[1] );
			double blue_diff = square( first_pixel[2] - second_pixel[2] );

			diff_sum += red_diff + green_diff + blue_diff;
			*/
			for (size_t dim_index = 0; dim_index < 3; ++dim_index) {
				diff_sum += square(second_pixel[dim_index] - first_pixel[dim_index]);
			}
		}
	}


	return diff_sum / area;
}

double stroke_mse (const Image &image, const colored_stroke &stroke, size_t step_number)
{
	auto stroke_color = stroke.background_color.to_OpenCV_Vec3();

	double total_diff_sum = 0;
	size_t points_in_stroke = 0;

	stroke.for_each(
		[&](size_t x, size_t y){
			const auto& image_pixel_color = image.at<cv::Vec3d>(y, x);

			for (size_t dim_index = 0; dim_index < 3; ++dim_index) {
				total_diff_sum += square(stroke_color[dim_index] - image_pixel_color[dim_index]);
			}

			points_in_stroke++;

		}, step_number, get_image_range_limits(image)
			);

	return total_diff_sum / points_in_stroke;
}
