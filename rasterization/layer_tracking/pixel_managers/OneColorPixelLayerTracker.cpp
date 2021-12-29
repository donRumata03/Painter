//
// Created by vova on 28.12.2021.
//

#include "OneColorPixelLayerTracker.h"

void OneColorPixelLayerTracker::add_color_layer(li priority, Color c) {
  // Accept new one if it has higher priority:
  if (!upper_color || priority > highest_priority) {
    upper_color = c;
    highest_priority = priority;
  }

  layers_count++;
}

size_t OneColorPixelLayerTracker::get_layers_count() const { return layers_count; }

std::optional<Color> OneColorPixelLayerTracker::get_pixel_color() const { return upper_color; }
