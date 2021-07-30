//
// Created by Vova on 16.07.2020.
//

#include "stroke_rasterizer.h"
#include "utils/Logger.h"

void rasterize_stroke (RasterizedPainting& target_painting, const colored_stroke &stroke, size_t point_number, StrokeRasterizationAlgorithm algo)
{
	auto opencv_stroke_color = stroke.background_color.to_OpenCV_Vec3();

	RangeRectangle<li> image_range_rectangle = get_image_range_limits(target_painting.layer_tracker);

	if (algo == StrokeRasterizationAlgorithm::smooth) {
		std::vector<cv::Point> cv_points;
		for (double t = 0; t <= 1; t += 1. / double(point_number)) {
			auto p = stroke.coords_at(t);

			if (image_range_rectangle.point_satisfies_requirements(p)) cv_points.emplace_back(p.x, p.y);
			else LogError("Rasterization") << "Problem: x = " << p.x << ", y = " << p.y <<
			                               " | rect (" << image_range_rectangle.min_x << ", " << image_range_rectangle.min_y << ", " << image_range_rectangle.max_x << ", " << image_range_rectangle.max_y << ")";
		}


		cv::polylines(target_painting.layer_tracker, cv_points, false, opencv_stroke_color, stroke.width);
		//cv::line(target_image, cv::Point(stroke.p0.x, stroke.p0.y), cv::Point(stroke.p2.x, stroke.p2.y), opencv_stroke_color, stroke.width);
	}
	else {
		stroke.for_each([&] (size_t x, size_t y) {
			target_painting.layer_tracker.at<cv::Vec3d>(y, x) = opencv_stroke_color;
		}, point_number, image_range_rectangle, algo);
	}

	auto stroke_float_rect = stroke.get_stroke_bounding_box();
	auto stroke_int_rect = convert_rect<li>(stroke_float_rect);
	image_range_rectangle.constrain_rect_to_fit_into_me(stroke_int_rect);

	auto unsigned_rectangle = image_range_rectangle.constrain_rect_to_fit_into_me()

	auto points = target_painting.get_pixel_list();

}

void rasterize_strokes (RasterizedPainting& target_painting, const std::vector<colored_stroke> &strokes, size_t point_number, StrokeRasterizationAlgorithm algo)
{
	for (const auto& stroke : strokes) rasterize_stroke(target_image, stroke, point_number, algo);
}


