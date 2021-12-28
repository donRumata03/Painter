#include "stroke_limits.h"

double StrokeLimits::image_dx() const {
  return image_rectangle.max_x - image_rectangle.min_x;
}

double StrokeLimits::image_dy() const {
  return image_rectangle.max_y - image_rectangle.min_y;
}

bool StrokeLimits::validate_self() const {
  if (min_sum >= max_dx + max_dy) return false;
  if (image_dx() < max_dx) return false;
  if (image_dy() < max_dy) return false;

  return true;
}

StrokeLimitsRequirementsResult StrokeLimits::stroke_satisfies_requirements(const Stroke& stroke) const {
  // Check width
  if (stroke.width < min_width or stroke.width > max_width) {
    return BadWidth;
  }

  RangeRectangle<double> stroke_bounding_box = stroke.get_curve_bounding_box();

  // Check being inside image
  if (!image_rectangle.check_other_being_fully_inside(stroke_bounding_box)) {
    return BoxNotFitIn;
  }
  auto dx = stroke_bounding_box.max_x - stroke_bounding_box.min_x;
  auto dy = stroke_bounding_box.max_y - stroke_bounding_box.min_y;

  if (!check_curvature(stroke)) {
    return BadCurvature;
  }

  return (dx + dy) >= min_sum and dx <= max_dx and dy <= max_dy ? Satisfied : NotFitIn;
}

bool StrokeLimits::constrain_stroke_to_requirements(Stroke& stroke) const {
  // Returns initial state of stroke
  if (stroke_satisfies_requirements(stroke) == StrokeLimitsRequirementsResult::Satisfied) return true;

  // Scale it
  constrain_stroke_size_parameters(stroke, min_sum, max_dx, max_dy,
                                   min_width, max_width);
  // Move it
  fit_stroke_into_rect(stroke, image_rectangle);

  // Constrain curvature of stroke


  return false;
}