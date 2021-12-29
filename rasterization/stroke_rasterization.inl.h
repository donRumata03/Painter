//
// Created by Vova on 29.12.2021.
//

#pragma once

template<typename PixelTracker>
void rasterize_stroke(RasterizedPainting<PixelTracker>& target_painting, const ColoredStroke& stroke, li layer_index,
                      size_t point_number, StrokeRasterizationAlgorithm algo) {
  rasterize_stroke(target_painting.cv_stroke_trap, stroke, point_number, algo);

  /// Prepare bounding-box:
  auto stroke_float_rect = stroke.get_stroke_bounding_box();
  auto stroke_int_rect = convert_rect<li>(stroke_float_rect);

  target_painting.process_pixels_from_trap(stroke_int_rect, layer_index);
}

template<typename PixelTracker>
void rasterize_strokes(RasterizedPainting<PixelTracker>& target_painting, const std::vector<ColoredStroke>& strokes,
                       size_t point_number, StrokeRasterizationAlgorithm algo) {
  for (li stroke_index = 0; stroke_index < strokes.size(); ++stroke_index) {
    rasterize_stroke(target_painting, strokes[stroke_index], stroke_index, point_number, algo);
  }
}



