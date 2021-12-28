#include "rasterized_paining.h"
#include "operations/image/figure.h"


RasterizedPainting::RasterizedPainting(cv::Size canvas_size, Color canvas_color)
        : m_canvas_color(canvas_color) {
  // image = make_default_image(w, h, canvas_color);
  cv_stroke_trap = make_default_image(canvas_size, canvas_color);
  layer_matrix = std::vector<std::vector<TreeMapPixelLayerTracker>>(
          canvas_size.height,
          std::vector<TreeMapPixelLayerTracker>(canvas_size.width, TreeMapPixelLayerTracker{})
          );
}

RasterizedPainting::PixelSet RasterizedPainting::get_pixel_list(const RangeRectangle<li>& bounding_box) {
  std::vector<ColoredPosition> res;

  for (size_t y = bounding_box.min_y; y < bounding_box.max_y; ++y) {
    for (size_t x = bounding_box.min_y; x < bounding_box.max_x; ++x) {
      auto pix_value = cv_stroke_trap.at<cv::Vec3d>(y, x);
      if (pix_value != m_canvas_color.to_OpenCV_Vec3()) {
        res.push_back(ColoredPosition{x, y, Color(pix_value)});
      }
    }
  }

  return res;
}

void RasterizedPainting::erase_trap_pixels(const RasterizedPainting::PixelSet& pixels) {
  for (const auto& pixel : pixels) {
    cv_stroke_trap.at<cv::Vec3d>(pixel.y, pixel.x) = m_canvas_color.to_OpenCV_Vec3();
  }
}

void RasterizedPainting::add_pixel_layer(const RasterizedPainting::PixelSet& pixels, size_t layer_index) {
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

void RasterizedPainting::process_pixels_from_trap(const RangeRectangle<li>& possible_bounding_box, li layer_index) {
  /// Get stroke pixels:
  auto pixels = get_painted_pixels(cv_stroke_trap, m_canvas_color);

  /// Use pixels:
  add_pixel_layer(pixels, layer_index);

  /// Delete pixels:
  erase_trap_pixels(pixels);
}

Image RasterizedPainting::render_image() {
  auto image = make_default_image(cv::Size{cv_stroke_trap.cols, cv_stroke_trap.rows,}, m_canvas_color);

  for (size_t y = 0; y < layer_matrix.size(); ++y) {
    for (size_t x = 0; x < layer_matrix[y].size(); ++x) {
      auto color = layer_matrix[y][x].get_pixel_color();
      if (color.has_value()) {
        image.at<cv::Vec3d>(y, x) = color->to_OpenCV_Vec3();
      }
    }
  }

  return image;
}

Image RasterizedPainting::get_imposition_matrix() const {
  auto image = make_default_image(cv::Size{cv_stroke_trap.cols, cv_stroke_trap.rows,}, m_canvas_color);
  assert(cv_stroke_trap.size == image.size);

  for (size_t y = 0; y < layer_matrix.size(); ++y) {
    for (size_t x = 0; x < layer_matrix[y].size(); ++x) {
      image.at<cv::Vec3d>(y, x) = std::clamp<double>(((double)layer_matrix[y][x].get_layers_count() - 1) / 3., 0, 1);
    }
  }

  return image;
}