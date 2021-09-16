#include "limits.h"
#include "data/stroke_limits.h"
#include "utils/logger.h"


bool is_fitting(const Stroke& stroke, const RangeRectangle<double>& rectangle) {
  return rectangle.point_satisfies_requirements(stroke.p0) and
         rectangle.point_satisfies_requirements(stroke.p1) and
         rectangle.point_satisfies_requirements(stroke.p2);
}

void shift_point(Point& point, const Point& shifting_vector) {
  point.x += shifting_vector.x;
  point.y += shifting_vector.y;
}

void shift_stroke(Stroke& stroke, const Point& shifting_vector) {
  shift_point(stroke.p0, shifting_vector);
  shift_point(stroke.p1, shifting_vector);
  shift_point(stroke.p2, shifting_vector);
}

void scale_stroke(Stroke& stroke, double factor) {
  stroke.p0 = stroke.p0 * factor;
  stroke.p1 = stroke.p1 * factor;
  stroke.p2 = stroke.p2 * factor;
  stroke.width *= factor;
}

bool segment_inside_segment(const std::pair<double, double>& inner, const std::pair<double, double>& outer) {
  return inner.first >= outer.first and inner.second <= outer.second;
}

double get_best_shift(const std::pair<double, double>& target_coords, const std::pair<double, double>& rect_coords,
                      double randomness_coefficient = 0.1) {

  if (target_coords.second <= target_coords.first || rect_coords.second <= rect_coords.first) {
    LogError("Limits", "get_best_shift") << "Something's wrong with rect or target coords: "
                                         << "Target coords: " << target_coords << ", Rect coords: " << rect_coords;
    assert(false);
  }

  if (segment_inside_segment(target_coords, rect_coords)) return 0;

  // TODO: add some randomness here to prevent concentrating many strokes on the border
  // double sigma = (rect_coords.second - rect_coords.first) * randomness_coefficient;

  // double movement_factor = randomness_coefficient + 1;
  // double res = 0;
  // return std::clamp(res * movement_factor, rect_coords.first, rect_coords.second);

  double delta = 0;

  if (target_coords.first < rect_coords.first) {
    // Move up
    delta = rect_coords.first - target_coords.first;
  } else {
    // Move down
    delta = rect_coords.second - target_coords.second;
  }

  return delta;
}


void fit_stroke_into_rect(Stroke& stroke, const RangeRectangle<double>& rectangle, bool croped) {
  if (is_fitting(stroke, rectangle)) return;

  if (!croped) {
    auto stroke_rectangle = stroke.get_curve_bounding_box();

    double dx = get_best_shift({stroke_rectangle.min_x, stroke_rectangle.max_x}, {rectangle.min_x, rectangle.max_x});
    double dy = get_best_shift({stroke_rectangle.min_y, stroke_rectangle.max_y}, {rectangle.min_y, rectangle.max_y});

    shift_stroke(stroke, {dx, dy});
  }

  rectangle.constrain_point(stroke.p0);
  rectangle.constrain_point(stroke.p1);
  rectangle.constrain_point(stroke.p2);
}

void constrain_stroke_size_parameters(Stroke& stroke, double min_dx, double max_dx, double min_dy, double max_dy,
                                      double min_width, double max_width) {
  auto bounding_box = stroke.get_curve_bounding_box();

  auto stroke_dx = bounding_box.max_x - bounding_box.min_x;
  auto stroke_dy = bounding_box.max_y - bounding_box.min_y;

  if (stroke_dx < min_dx) stroke.scale_x_from_center(min_dx / stroke_dx);
  if (stroke_dx > max_dx) stroke.scale_x_from_center(max_dx / stroke_dx);

  if (stroke_dy < min_dy) stroke.scale_y_from_center(min_dy / stroke_dy);
  if (stroke_dy > max_dy) stroke.scale_y_from_center(max_dy / stroke_dy);

  // Width
  stroke.width = std::clamp(stroke.width, min_width, max_width);
}
