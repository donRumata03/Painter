#pragma once


#include <ostream>
#include "data/stroke.h"
#include "data/range_rectangle.h"
#include "operations/stroke/limits.h"


struct StrokeLimits {
  double min_dx, max_dx; /// delta x
  double min_dy, max_dy; /// delta y

  double min_width, max_width; /// width

  RangeRectangle<double> image_rectangle;

  [[nodiscard]] double image_dx() const;

  [[nodiscard]] double image_dy() const;

  [[nodiscard]] bool validate_self() const;

  [[nodiscard]] bool stroke_satisfies_requirements(const Stroke& stroke) const;

  // TODO: Rework with constraining by rotated bounding box
  bool constrain_stroke_to_requirements(Stroke& stroke) const;

  friend std::ostream& operator<<(std::ostream& os, const StrokeLimits& limits) {
    os << "Stroke limits { " << "min_dx: " << limits.min_dx << " max_dx: " << limits.max_dx
       << " min_dy: " << limits.min_dy << " max_dy: " << limits.max_dy << " min_width: "
       << limits.min_width << " max_width: " << limits.max_width << " image_rectangle: "
       << limits.image_rectangle << " }";
    return os;
  }
};

