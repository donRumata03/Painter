#include "pixel_layer_tracker.h"


std::optional<Color> PixelLayerTracker::get_pixel_color() {
  if (m_data.empty()) {
    return {};
  }

  // TODO: add color blending!
  return m_data.rbegin()->second;
}

void PixelLayerTracker::add_color_layer(li priority, Color c) {
  m_data[priority] = c;
}

Color PixelLayerTracker::remove_layer_by_priority(li priority) {
  auto it = m_data.find(priority);
  if (it == m_data.end()) {
    throw std::runtime_error("Can't find layer with given priority!");
  }

  Color c = it->second;
  m_data.erase(it);

  return c;
}
