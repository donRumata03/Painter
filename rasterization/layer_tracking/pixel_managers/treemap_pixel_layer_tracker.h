#pragma once

#include "painter_pch.h"
#include "utils/image.h"
#include "pixel_layer_tracker.h"

/**
 * Stores all color layers with priority for single pixel in tree map (sorted by priority)
 * Ultimative (contains all information; asymptotically optimal) but slow
 */
class TreeMapPixelLayerTracker {
 private:
  std::map<li, Color> m_data = {};

 public:
	// Constructs defaultly //
	TreeMapPixelLayerTracker() : m_data() {}

  void add_color_layer(li priority, Color c);

  Color remove_layer_by_priority(li priority);

  size_t get_layers_count() const;

  std::optional<Color> get_pixel_color();
};

static_assert(PixelLayerTracker<TreeMapPixelLayerTracker>);
