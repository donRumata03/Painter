#pragma once


#include <ostream>
#include "data/stroke.h"
#include "data/range_rectangle.h"
#include "operations/stroke/limits.h"


enum StrokeLimitsRequirementsResult {
  Satisfied = 0,
  BadWidth,
  BadCurvature,
  NotFitIn,
  BoxNotFitIn
};


struct StrokeLimits {
  double min_sum; /// min delta
  double max_dx, max_dy; /// max delta

  double min_width, max_width; /// width

  RangeRectangle<double> image_rectangle;

  [[nodiscard]] double image_dx() const;

  [[nodiscard]] double image_dy() const;

  [[nodiscard]] bool validate_self() const;

  [[nodiscard]] StrokeLimitsRequirementsResult stroke_satisfies_requirements(const Stroke& stroke) const;

  // TODO: Rework with constraining by rotated bounding box
  bool constrain_stroke_to_requirements(Stroke& stroke) const;

  friend std::ostream& operator<<(std::ostream& os, const StrokeLimits& limits) {
    os << "Stroke limits { " << "min_sum: " << limits.min_sum<< " max_dx: " << limits.max_dx
       << " min_dy: " << limits.max_dy << " min_width: "
       << limits.min_width << " max_width: " << limits.max_width << " image_rectangle: "
       << limits.image_rectangle << " }";
    return os;
  }
};

