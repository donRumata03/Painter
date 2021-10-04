#pragma once

#include <utility>

#include "painter_pch.h"
#include "data/color.h"


struct VectorRegion {
  Image image;
  cv::Rect box;
  Color color;
  std::string path;

  VectorRegion() = default;

  explicit VectorRegion(Image image, cv::Rect& box, Color color, const std::string& path) : image(std::move(image)), box(box),
                                                                                             color(color), path(path) {}
};