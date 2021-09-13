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

  [[nodiscard]] double image_dx() const { return image_rectangle.max_x - image_rectangle.min_x; }

  [[nodiscard]] double image_dy() const { return image_rectangle.max_y - image_rectangle.min_y; }

  [[nodiscard]] bool validate_self() const {
    if (min_dx >= max_dx) return false;
    if (min_dy >= max_dy) return false;
    if (image_dx() < max_dx) return false;
    if (image_dy() < max_dy) return false;

    return true;
  }

  [[nodiscard]] bool stroke_satisfies_requirements(const Stroke& stroke) const {
    // Check width
    if (stroke.width < min_width or stroke.width > max_width) return false;

    RangeRectangle<double> stroke_bounding_box = stroke.get_curve_bounding_box();

    // Check being inside image
    if (not image_rectangle.check_other_being_fully_inside(stroke_bounding_box))
      return false;

    auto dx = stroke_bounding_box.max_x - stroke_bounding_box.min_x;
    auto dy = stroke_bounding_box.max_y - stroke_bounding_box.min_y;

    return dx <= max_dx and dx >= min_dx and dy <= max_dy and dy >= min_dy;
  }

  bool constrain_stroke_to_requirements(Stroke& stroke) const {
    // Returns initial state of stroke
    if (stroke_satisfies_requirements(stroke)) return true;

    // Scale it
    constrain_stroke_size_parameters(stroke, min_dx, max_dx, min_dy, max_dy, min_width, max_width);

    // Constrain curvature of stroke
    point middle = (stroke.p0 + stroke.p2) / 2;
    double max_dist = point::dist(stroke.p0, stroke.p2) / 2;
    point vec = middle - stroke.p1;

    if (vec.module() > max_dist) {
      stroke.p1 = stroke.p1 + vec * (vec.module() - max_dist) / vec.module();
    }

    // Move it
    fit_stroke_into_rect(stroke, image_rectangle);


    return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const StrokeLimits& limits) {
    os << "Stroke limits { " << "min_dx: " << limits.min_dx << " max_dx: " << limits.max_dx
       << " min_dy: " << limits.min_dy << " max_dy: " << limits.max_dy << " min_width: "
       << limits.min_width << " max_width: " << limits.max_width << " image_rectangle: "
       << limits.image_rectangle << " }";
    return os;
  }
};
