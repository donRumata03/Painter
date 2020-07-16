//
// Created by Vova on 16.07.2020.
//

#include "stroke_rasterizer.h"

void rasterize_stroke (Image &target_image, const stroke &stroke, size_t point_number)
{
	for (size_t point_index = 0; point_index < point_number; ++point_index) {
		double t = double(point_index) / point_number;

		auto [central_x, central_y] = stroke.coords_at(t);
		double height = stroke.height_at(t);

		auto y0 = size_t(std::round(std::max(0., central_y - height)));
		auto y1 = size_t(std::round(std::min(double(target_image.rows), central_y - height)));

		// Draw the line:
		auto x = int(std::round(central_x));
		for (size_t y = y0; y < y1; ++y) {
			target_image.at<cv::Vec3d>(x, y) = stroke.background_color.to_OpenCV_Vec3();
		}
	}
}

void rasterize_strokes (Image &target_image, const std::vector<stroke> &strokes)
{
	for (const auto& stroke : strokes) rasterize_stroke(target_image, stroke);
}


