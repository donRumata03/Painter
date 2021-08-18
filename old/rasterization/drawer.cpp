//
// Created by Vova on 16.07.2020.
//

#include "drawer.h"

Image &draw_circle (Image &image, circle &circle, color circle_color)
{
	cv::circle(
			image,
			cv::Point{ int(std::round(circle.center.x)), int(std::round(circle.center.y)) }, int(std::round(circle.r)),
			circle_color.to_OpenCV_Scalar(), cv::FILLED
			);

	return image;
}

Image &
draw_line (Image &image, const std::pair<lint, lint> &start, const std::pair<lint, lint> &end, const color &line_color,
           size_t thickness)
{
	cv::Point first = { int(start.first), int(start.second) };
	cv::Point second = { int(end.first), int(end.second) };

	auto image_rect = cv::Size{ image.size[0], image.size[1] };
	cv::clipLine(image_rect, first, second);

	draw_line_unchecked(image, { first.x, first.y }, { second.x, second.y }, line_color, thickness);

	return image;
}

Image &draw_line_unchecked (Image &image, const std::pair<lint, lint> &start, const std::pair<lint, lint> &end,
                            const color &line_color, size_t thickness)
{
	cv::Point first = { int(start.first), int(start.second) };
	cv::Point second = { int(end.first), int(end.second) };

	cv::line(
			image,
			first, second,
			line_color.to_OpenCV_Scalar(), int(thickness)
	);


	return image;
}
