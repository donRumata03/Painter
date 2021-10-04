#include "stroke.h"
#include "utils/logger.h"


void rasterize_stroke(Image& target_image, const ColoredStroke& stroke, size_t point_number,
                      StrokeRasterizationAlgorithm algo) {
  if (stroke.width <= 0) {
    LogError("Rasterization") << "Invalid stroke width: " << stroke;
    return;
  }

  auto opencv_stroke_color = stroke.background_color.to_OpenCV_Vec3();

  RangeRectangle<li> image_range_rectangle = get_image_range_limits(target_image);

  if (algo == StrokeRasterizationAlgorithm::smooth) {
    std::vector<cv::Point> cv_points;
    for (double t = 0; t <= 1; t += 1. / double(point_number)) {
      auto p = stroke.coords_at(t);

      if (image_range_rectangle.point_satisfies_requirements(p))
        cv_points.emplace_back(int(std::round(p.x)), int(std::round(p.y)));
      else
        LogError("Rasterization") << "Not in rect: " << p << " | " << image_range_rectangle;
    }


    cv::polylines(target_image, cv_points, false, opencv_stroke_color, stroke.width);
  } else {
    stroke.for_each([&](size_t x, size_t y) {
      target_image.at<cv::Vec3d>(y, x) = opencv_stroke_color;
    }, point_number, image_range_rectangle, algo);
  }
}


void rasterize_strokes(Image& target_image, const std::vector<ColoredStroke>& strokes, size_t point_number,
                       StrokeRasterizationAlgorithm algo) {
  for (const auto& stroke : strokes) rasterize_stroke(target_image, stroke, point_number, algo);
}

void rasterize_stroke(RasterizedPainting& target_painting, const ColoredStroke& stroke, li layer_index,
                      size_t point_number, StrokeRasterizationAlgorithm algo) {
  rasterize_stroke(target_painting.cv_stroke_trap, stroke, point_number, algo);

  /// Prepare bounding-box:
  auto stroke_float_rect = stroke.get_stroke_bounding_box();
  auto stroke_int_rect = convert_rect<li>(stroke_float_rect);

  target_painting.process_pixels_from_trap(stroke_int_rect, layer_index);
}

void rasterize_strokes(RasterizedPainting& target_painting, const std::vector<ColoredStroke>& strokes,
                       size_t point_number, StrokeRasterizationAlgorithm algo) {
  for (li stroke_index = 0; stroke_index < strokes.size(); ++stroke_index) {
    rasterize_stroke(target_painting, strokes[stroke_index], stroke_index, point_number, algo);
  }
}


