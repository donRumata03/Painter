#pragma once

#include "painter_pch.h"
#include "units.h"


struct TransformImageData {
  Point move_vector;
  double scale_factor;
};


class Canvas {
 private:
  size_t mm_width = 0;
  size_t mm_height = 0;

  size_t _dpi = 96;

 public:
  Canvas() = default;

  Canvas(size_t mm_width, size_t mm_height, size_t dpi = 96) : mm_width(mm_width), mm_height(mm_height), _dpi(dpi) {}

  [[nodiscard]] size_t width(Units units = Units::PX) const {
    return units == Units::PX ? (size_t) std::round(mm2px(mm_width)) : mm_width;
  }

  [[nodiscard]] size_t height(Units units = Units::PX) const {
    return units == Units::PX ? (size_t) std::round(mm2px(mm_height)) : mm_height;
  }

  [[nodiscard]] size_t dpi() const { return _dpi; }

  double mm2px(double mm) const { return mm / kMmPerInch * double(_dpi); }
  double px2mm(double px) const { return px * kMmPerInch / double(_dpi); }

  static Canvas get_default_canvas() { return Canvas(kDefaultCanvasWidth, kDefaultCanvasHeight, kDefaultCanvasDpi); }

  auto operator<=>(const Canvas& canvas) const = default;
};

TransformImageData calc_transform_to_canvas(const Canvas& canvas, size_t w, size_t h);

void to_json(json& j, const Canvas& canvas);
void from_json(const json& j, Canvas& canvas);

