#pragma once

#include "data/range_rectangle.h"
#include "utils/image.h"
#include "rasterization/pixel_layer_tracker.h"


struct ColoredPosition {
  li y = 0;
  li x = 0;
  Color c{};
};


struct RasterizedPainting {
  using PixelSet = std::vector<ColoredPosition>;


  // Image image;

  Image cv_stroke_trap;
  std::vector<std::vector<PixelLayerTracker>> layer_matrix;

  Color m_canvas_color;

 public:
  explicit RasterizedPainting(li h, li w, Color canvas_color);

  /// Working with pixel lists
  PixelSet get_pixel_list(const RangeRectangle<li>& bounding_box);

  void erase_trap_pixels(const PixelSet& pixels);

  // void paint_pixels(const PixelSet& pixels);
  void add_pixel_layer(const PixelSet& pixels, size_t layer_index);

  void process_pixels_from_trap(const RangeRectangle<li>& possible_bounding_box, li layer_index);

  /// Indexing
  PixelLayerTracker& layers_at(li y, li x) { return layer_matrix[y][x]; }

  Color color_at(li y, li x) { return *layers_at(y, x).get_pixel_color(); }

  Image render_image();
};

