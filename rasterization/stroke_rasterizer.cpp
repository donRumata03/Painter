//
// Created by Vova on 16.07.2020.
//

#include "stroke_rasterizer.h"
#include "utils/Logger.h"

void rasterize_stroke (Image &target_image, const colored_stroke &stroke, size_t point_number)
{
	auto opencv_stroke_color = stroke.background_color.to_OpenCV_Vec3();

	RangeRectangle<lint> range_rectangle = get_image_range_limits(target_image);

	std::vector<cv::Point> cv_points;
	for (double t = 0; t <= 1; t += 0.01)
    {
	    auto p = stroke.coords_at(t);/*
	    if (!range_rectangle.point_satisfies_requirements(p)) {
	        LogError("Rasterization") << "Problem: x = " << p.x << ", y = " << p.y <<
                " | rect (" << range_rectangle.min_x << ", " << range_rectangle.min_y << ", " << range_rectangle.max_x << ", " << range_rectangle.max_y << ")";
	    }*/
	    if (range_rectangle.point_satisfies_requirements(p)) cv_points.emplace_back(cv::Point(p.x, p.y));
    }


	cv::polylines(target_image, cv_points, false, opencv_stroke_color, stroke.width);

/*
	stroke.for_each([&](size_t x, size_t y){
		target_image.at<cv::Vec3d>(y, x) = opencv_stroke_color;
	}, point_number, range_rectangle);*/
}

void rasterize_strokes (Image &target_image, const std::vector<colored_stroke> &strokes)
{
	for (const auto& stroke : strokes) rasterize_stroke(target_image, stroke);
}


