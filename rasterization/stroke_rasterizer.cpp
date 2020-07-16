//
// Created by Vova on 16.07.2020.
//

#include "stroke_rasterizer.h"

void rasterize_stroke (Image &target_image, const stroke &stroke, size_t point_number)
{
	auto opencv_color = stroke.background_color.to_OpenCV_Vec3();

	Range_rectangle<lint> range_rectangle = {
			0, target_image.cols, 0, target_image.rows
	};

	stroke.for_each([&](size_t x, size_t y){
		target_image.at<cv::Vec3d>(x, y) = opencv_color;
	}, point_number, range_rectangle);


}

void rasterize_strokes (Image &target_image, const std::vector<stroke> &strokes)
{
	for (const auto& stroke : strokes) rasterize_stroke(target_image, stroke);
}


