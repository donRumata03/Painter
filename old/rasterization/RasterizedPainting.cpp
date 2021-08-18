//
// Created by Vova on 22.07.2021.
//

#include "RasterizedPainting.h"



RasterizedPainting::RasterizedPainting (li h, li w, color canvas_color)
		: m_canvas_color(canvas_color)
{
	// image = make_default_image(w, h, canvas_color);
	cv_stroke_trap = make_default_image(w, h, canvas_color);
	layer_matrix = std::vector<std::vector<PixelLayerTracker>>(
			h, std::vector<PixelLayerTracker>(w, PixelLayerTracker{})
	);
}

RasterizedPainting::PixelSet RasterizedPainting::get_pixel_list (const RangeRectangle<li>& bounding_box)
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

void RasterizedPainting::erase_trap_pixels (const RasterizedPainting::PixelSet& pixels)
{
	for (const auto& pixel : pixels) {
		cv_stroke_trap.at<cv::Vec3d>(pixel.y, pixel.x) = m_canvas_color.to_OpenCV_Vec3();
	}
}

void RasterizedPainting::add_pixel_layer (const RasterizedPainting::PixelSet& pixels, size_t layer_index)
{
	for (const auto& pixel : pixels) {
		layer_matrix[pixel.y][pixel.x].add_color_layer(layer_index, pixel.c);
	}
}

//void RasterizedPainting::paint_pixels (const RasterizedPainting::PixelSet& pixels)
//{
//	for (const auto& pixel : pixels) {
//		image.at<cv::Vec3d>(pixel.y, pixel.x) = pixel.c.to_OpenCV_Vec3();
//	}
//}

void RasterizedPainting::process_pixels_from_trap (const RangeRectangle<li>& possible_bounding_box, li layer_index)
{
	RangeRectangle<li> image_range_rectangle = get_image_range_limits(cv_stroke_trap);

	auto bounding_box = possible_bounding_box;
	image_range_rectangle.constrain_rect_to_fit_into_me(bounding_box);

	/// Get stroke pixels:
	auto pixels = get_pixel_list(bounding_box);

	/// Use pixels:
	add_pixel_layer(pixels, layer_index);
	// paint_pixels(pixels);

	/// Delete pixels:
	erase_trap_pixels(pixels);
}


///										______________________________

Image RasterizedPainting::render_image ()
{
	auto image = make_default_image(
			cv::Size {
				cv_stroke_trap.cols,
				cv_stroke_trap.rows,
			},
			m_canvas_color
			);

	for (li y = 0; y < layer_matrix.size(); ++y) {
		for (li x = 0; x < layer_matrix[y].size(); ++x) {
			image.at<cv::Vec3d>(y, x) = layer_matrix[y][x].get_pixel_color()->to_OpenCV_Vec3();
		}
	}

	return image;
}

