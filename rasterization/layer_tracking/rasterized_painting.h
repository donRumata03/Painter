#pragma once

#include "data/range_rectangle.h"
#include "utils/image.h"
#include "rasterization/layer_tracking/pixel_managers/pixel_layer_tracker.h"
#include "rasterization/layer_tracking/pixel_managers/treemap_pixel_layer_tracker.h"


template<typename Tracker> requires PixelLayerTracker<Tracker>
struct RasterizedPainting {
  using PixelSet = std::vector<ColoredPosition>;


  // Image image;

  Image cv_stroke_trap;
  std::vector<std::vector<Tracker>> layer_matrix;

  Color m_canvas_color;

 public:
    explicit RasterizedPainting(cv::Size canvas_size, Color canvas_color);

  /// Working with pixel lists
  PixelSet get_pixel_list(const RangeRectangle<li>& bounding_box);

  void erase_trap_pixels(const PixelSet& pixels);

  // void paint_pixels(const PixelSet& pixels);
  void add_pixel_layer(const PixelSet& pixels, size_t layer_index);

  void process_pixels_from_trap(const RangeRectangle<li>& possible_bounding_box, li layer_index);

  /// Indexing
  TreeMapPixelLayerTracker& layers_at(li y, li x) { return layer_matrix[y][x]; }

  Color color_at(li y, li x) { return *layers_at(y, x).get_pixel_color(); }

  Image render_image();

  Image get_imposition_matrix() const;
};


#include "rasterized_painting.inl.h"