//
// Created by vova on 28.12.2021.
//

#include "SortAfterFillingPixelLayerTracker.h"

size_t SortAfterFillingPixelLayerTracker::get_layers_count() const {
  return brush_layers.size();
}

void SortAfterFillingPixelLayerTracker::add_color_layer(li priority, Color c) {
  if (!brush_layers.empty() && brush_layers.back().priority > priority) {
    is_sorted = false;
  }

  brush_layers.push_back({
    .priority = priority,
    .color = c
  });
}

std::optional<Color> SortAfterFillingPixelLayerTracker::get_pixel_color() {
  if (!is_sorted) {
    throw std::runtime_error("Can't get color with highest priority while not sorted");
  }

  return brush_layers.empty() ? std::nullopt : std::optional(brush_layers.back().color);
}

void SortAfterFillingPixelLayerTracker::sort_by_priority() {
  std::sort(brush_layers.begin(), brush_layers.end(), [](const PixelPaintingData& left, const PixelPaintingData& right){
    return left.priority > right.priority;
  });
}
