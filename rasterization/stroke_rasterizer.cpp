//
// Created by Vova on 16.07.2020.
//

#include "stroke_rasterizer.h"

void rasterize_stroke (Image &target_image, const colored_stroke &stroke, size_t point_number)
{
	auto opencv_stroke_color = stroke.background_color.to_OpenCV_Vec3();

	RangeRectangle<lint> range_rectangle = get_image_range_limits(target_image);

	stroke.for_each([&](size_t x, size_t y){
		target_image.at<cv::Vec3d>(y, x) = opencv_stroke_color;
	}, point_number, range_rectangle);
}

void rasterize_strokes (Image &target_image, const std::vector<colored_stroke> &strokes)
{
	for (const auto& stroke : strokes) rasterize_stroke(target_image, stroke);
}


