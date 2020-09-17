//
// Created by Vova on 16.07.2020.
//

#include "error_computing.h"

#include <execution>

double image_mse (const Image &image1, const Image &image2, bool parallel)
{
	assert(image1.size == image2.size);
	size_t w = image1.cols, h = image1.rows;
	auto area = double(w * h);

	double diff_sum = 0;

	if (not parallel) {
		for (size_t x = 0; x < w; ++x) {
			for (size_t y = 0; y < h; ++y) {
				const auto &first_pixel = image1.at<cv::Vec3d>(y, x);
				const auto &second_pixel = image2.at<cv::Vec3d>(y, x);

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
	}
	else { // Parallel:
		std::atomic<double> atomic_diff_sum = 0;

		image1.forEach<Pixel>([&](Pixel& first_pixel_value, const int position[]){
			// TODO: x, y or y, x? => define first and second pixels
			double this_diff_sum = 0; // atomic_diff_sum.load();

			const auto first_pixel = color(first_pixel_value);
			const auto second_pixel = color(image2.at<cv::Vec3d>(position[0], position[1]));


			for (size_t dim_index = 0; dim_index < 3; ++dim_index) {
				this_diff_sum += square(second_pixel[dim_index] - first_pixel[dim_index]);
			}

			// atomic_diff_sum.store(this_diff_sum);
			atomic_diff_sum += this_diff_sum;
		});

		diff_sum = atomic_diff_sum;
	}

	return diff_sum / area;
}

double stroke_mse (const Image &image, const colored_stroke &stroke, size_t step_number, bool parallel)
{
	return stroke_mse(image, stroke.get_points(step_number, get_image_range_limits(image)), stroke.background_color, parallel);
}

double stroke_mse (const Image &image, const std::vector<stroke::point> &stroke_points, const color &stroke_color, bool parallel)
{
	auto stroke_color_vec = stroke_color.to_OpenCV_Vec3();

	double total_diff_sum = 0;
	size_t points_in_stroke = 0;

	auto point_handler = [&](const stroke::point& point_xy){
		size_t x = point_xy.x;
		size_t y = point_xy.y;
		const auto& image_pixel_color = image.at<cv::Vec3d>(y, x);

		for (size_t dim_index = 0; dim_index < 3; ++dim_index) {
			total_diff_sum += square(stroke_color[dim_index] - image_pixel_color[dim_index]);
		}

		points_in_stroke++;

	};

	if (parallel) {
		std::for_each(std::execution::par_unseq, stroke_points.begin(),
		              stroke_points.end(), point_handler);
	}
	else {
		std::for_each(std::execution::seq, stroke_points.begin(),
		              stroke_points.end(), point_handler);
	}

	return total_diff_sum / points_in_stroke;
}
