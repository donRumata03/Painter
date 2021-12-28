#pragma once

#include "utils/image.h"


/**
 * Stores color layers with priority for single pixel
 */
class PixelLayerTracker {
 private:
  std::map<li, Color> m_data;

 public:
  void add_color_layer(li priority, Color c);

  Color remove_layer_by_priority(li priority);

  size_t get_layers_count() const;

  std::optional<Color> get_pixel_color();
};
