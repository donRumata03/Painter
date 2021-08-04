//
// Created by Vova on 22.07.2021.
//

#include "RasterizedPainting.h"



RasterizedPainting::RasterizedPainting (size_t h, size_t w, color canvas_color)
		: m_canvas_color(canvas_color)
{
	image = make_default_image(w, h, canvas_color);
	cv_stroke_trap = make_default_image(w, h, canvas_color);
	layer_matrix = std::vector<std::vector<size_t>>(
			image.rows, std::vector<size_t>(image.cols, PixelLayerTracker{})
	);
}

RasterizedPainting::PixelSet RasterizedPainting::get_pixel_list (const RangeRectangle<size_t>& bounding_box)
{
	std::vector<ColoredPosition> res;

	for (li y = bounding_box.min_y; y < bounding_box.max_y; ++y) {
		for (li x = bounding_box.min_y; x < bounding_box.max_x; ++x) {
			auto pix_value = cv_stroke_trap.at<cv::Vec3d>(y, x);
			if (pix_value != m_canvas_color.to_OpenCV_Vec3()) {
				res.push_back(ColoredPosition{ y, x, color {pix_value} });
			}
		}
	}

	return res;
}

void RasterizedPainting::erase_pixels (const RasterizedPainting::PixelSet& pixels)
{
	for (const auto& pixel : pixels) {
		cv_stroke_trap.at<cv::Vec3d>(pixel.y, pixel.x) = m_canvas_color.to_OpenCV_Vec3();
	}
}

void RasterizedPainting::add_pixel_layers (const RasterizedPainting::PixelSet& pixels, size_t layer_index)
{
	for (const auto& pixel : pixels) {
		layer_matrix[pixel.y][pixel.x].add_color_layer(layer_index, pixel.c);
	}
}

void RasterizedPainting::paint_pixels (const RasterizedPainting::PixelSet& pixels)
{
	for (const auto& pixel : pixels) {
		image.at<cv::Vec3d>(pixel.y, pixel.x) = pixel.c.to_OpenCV_Vec3();
	}
}

