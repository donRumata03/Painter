//
// Created by Vova on 22.07.2021.
//

#include "RasterizedPainting.h"

RasterizedPainting::RasterizedPainting (size_t h, size_t w, color canvas_color)
		: m_canvas_color(canvas_color)
{
	image = make_default_image(w, h, canvas_color);
	layer_tracker = make_default_image(w, h, canvas_color);
	layer_matrix = std::vector<std::vector<size_t>>(
			image.rows, std::vector<size_t>(image.cols, size_t(0))
	);
}

RasterizedPainting::PixelSet RasterizedPainting::get_pixel_list (const RangeRectangle<size_t>& bounding_box)
{
	std::vector<std::pair<size_t, size_t>> res;

	for (size_t y = bounding_box.min_y; y < bounding_box.max_y; ++y) {
		for (size_t x = bounding_box.min_y; x < bounding_box.max_x; ++x) {
			if (layer_tracker.at<cv::Vec3d>(y, x) != m_canvas_color.to_OpenCV_Vec3()) {
				res.emplace_back(y, x);
			}
		}
	}

	return res;
}

void RasterizedPainting::erase_pixels (const RasterizedPainting::PixelSet& pixels)
{
	for (const auto& pixel : pixels) {
		layer_tracker.at<cv::Vec3d>(pixel.first, pixel.second) = m_canvas_color.to_OpenCV_Vec3();
	}
}

void RasterizedPainting::apply_layers (const RasterizedPainting::PixelSet& pixels)
{
	for (const auto& pixel : pixels) {
		layer_matrix[pixel.first][pixel.second] += 1;
	}
}
