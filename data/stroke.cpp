#include "stroke.h"


Point Stroke::coords_at(double t) const {
  assert(t >= 0 && t <= 1);

  return p1 + square(1 - t) * (p0 - p1) + square(t) * (p2 - p1);
}

double Stroke::height_at(double t) const {
  assert(t >= 0 && t <= 1);

  // TODO?

  return width;
}


Point Stroke::derivative_at(double t) const {
  Point res = 2 * (t - 1) * (p0 - p1) + 2 * t * (p2 - p1);
  return res;
}

double Stroke::t_at(const Point& point_in_stroke) const {
  auto is_good_t = [](double t_candidate) -> bool { return t_candidate >= 0 and t_candidate <= 1; };

  Point the_result;

  if ((p0 - 2 * p1 + p2).is_zero()) {
    if (p0 != p1) {
      the_result = (point_in_stroke - p0) / (2 * (p1 - p0));
    } else { // p0 == p1 != p2
      Point for_sqrt = (point_in_stroke - p0) / (p2 - p1);
      assert(for_sqrt.x != 0 and for_sqrt.y != 0);
      the_result = sqrt(for_sqrt);
    }

    if (is_good_t(the_result.x)) return the_result.x;
    assert(is_good_t(the_result.y));
    return the_result.y;
  } else { // p0 - 2p1 + p2 != 0
    Point for_sqrt = (p0 - 2 * p1 + p2) * point_in_stroke + p1 * p1 - p0 * p2;
    assert(for_sqrt.x != 0 and for_sqrt.y != 0);
    Point the_sqr = sqrt(for_sqrt);


    Point solution1 = (p0 - p1 + the_sqr) / (p0 - 2 * p1 + p2);
    Point solution2 = (p0 - p1 - the_sqr) / (p0 - 2 * p1 + p2);

    std::vector<double> solutions = {solution1.x, solution1.y, solution2.x, solution2.y};
    std::sort(solutions.begin(), solutions.end());

    size_t solution_index = 0;
    for (auto& possible_t : solutions) {
      if (solution_index != solutions.size() - 1 and is_good_t(possible_t) and
          almost_equal(possible_t, solutions[solution_index + 1], 1e-7)) {
        return possible_t;
      }
      solution_index++;
    }

    // If didn't find 2 appropriate solutions, try looking for only one
    for (auto& possible_t : solutions) {
      if (is_good_t(possible_t)) {
        return possible_t;
      }
    }

    assert(false);
  }


  return -1;
}

/**
 * @see https://malczak.linuxpl.com/blog/quadratic-bezier-curve-length/ for formula derivation
 * @return The length of the stroke
 */
double Stroke::length() const {
  Point a = p0 - 2 * p1 + p2;
  Point b = 2 * (p1 - p0);

  double A = 4 * squared_abs(a);
  double B = 4 * (a.x * b.x + a.y * b.y);
  double C = squared_abs(b);

  double S_abc = 2 * sqrt(A + B + C);
  double A_2 = sqrt(A);
  double A_32 = 2 * A * A_2;
  double C_2 = 2 * sqrt(C);
  double BA = B / A_2;

  return (A_32 * S_abc + A_2 * B * (S_abc - C_2) + (4 * C * A - B * B) * log((2 * A_2 + BA + S_abc) / (BA + C_2)))
         / (4 * A_32);
}


std::vector<point> Stroke::get_points(
        size_t step_number, std::optional<RangeRectangle<lint>> range_limits, StrokeRasterizationAlgorithm algo) const {
  std::vector<Point> res;

  this->for_each(
          [&](lint x, lint y) {
            res.emplace_back(double(x), double(y));
          },
          step_number, range_limits);

  return res;
}

std::ostream& operator<<(std::ostream& os, const Stroke& stroke) {
  os << "Stroke { p0: " << stroke.p0 << " p1: " << stroke.p1 << " p2: " << stroke.p2 << " width: " << stroke.width
     << " }";
  return os;
}


void Stroke::scale_x_from_center(double scale_factor) {
  double center_x = center().x;

  p0.scale_x_from(center_x, scale_factor);
  p1.scale_x_from(center_x, scale_factor);
  p2.scale_x_from(center_x, scale_factor);
}

void Stroke::scale_y_from_center(double scale_factor) {
  double center_y = center().y;

  p0.scale_y_from(center_y, scale_factor);
  p1.scale_y_from(center_y, scale_factor);
  p2.scale_y_from(center_y, scale_factor);
}

void Stroke::scale_from_center(double scale_factor) {
  Point center = this->center();

  p0.scale_from(center, scale_factor);
  p1.scale_from(center, scale_factor);
  p2.scale_from(center, scale_factor);
}

RangeRectangle<double> Stroke::get_curve_bounding_box() const {
  RangeRectangle<double> res{};

  res.min_x = std::numeric_limits<double>::max();
  res.min_y = std::numeric_limits<double>::max();
  res.max_x = std::numeric_limits<double>::min();
  res.max_y = std::numeric_limits<double>::min();

  auto process_point = [&](const Point& p) {
    res.min_x = std::min(res.min_x, p.x);
    res.min_y = std::min(res.min_y, p.y);

    res.max_x = std::max(res.max_x, p.x);
    res.max_y = std::max(res.max_y, p.y);
  };

  process_point(p0);
  process_point(p1);
  process_point(p2);

  return res;
}

RangeRectangle<double> Stroke::get_stroke_bounding_box() const {
  RangeRectangle<double> box = get_curve_bounding_box();

  double padding = (width / 2) * 1.1;

  box.min_y -= padding;
  box.min_x -= padding;

  box.max_y += padding;
  box.max_x += padding;

  return box;
}


void to_json(json& j, const ContextWrapper<Stroke>& wrapped_stroke) {
  Stroke temp_stroke = wrapped_stroke.object;
  auto invert_y = [&](Point& p) {
    p.y = wrapped_stroke.canvas.height(Units::MM) - p.y;
  };

  invert_y(temp_stroke.p0);
  invert_y(temp_stroke.p1);
  invert_y(temp_stroke.p2);

  j["type"] = "quadratic_bezier";
  j["p0"] = temp_stroke.p0;
  j["p1"] = temp_stroke.p1;
  j["p2"] = temp_stroke.p2;
  j["width"] = temp_stroke.width;
}

void to_json(json& j, const ContextWrapper<ColoredStroke>& col_stroke) {
  to_json(j, (ContextWrapper<Stroke>&) col_stroke);

  j["color"] = convert_color<uint8_t>(col_stroke.object.background_color);
}


void to_json(json& j, const ContextWrapper<std::vector<ColoredStroke>>& strokes) {
  for (size_t i = 0; i < strokes.object.size(); i++) {
    j[i] = ContextWrapper<ColoredStroke>{strokes.object[i], strokes.canvas};
  }
}

void from_json(const json& j, Stroke& target_stroke, const Canvas& canvas) {
  assert(j["type"] == "quadratic_bezier");

  target_stroke = Stroke(j["p0"].get<Point>(), j["p1"].get<Point>(), j["p2"].get<Point>(), j["width"].get<double>());

  auto invert_y = [&](point& p) {
    p.y = canvas.height(Units::MM) - p.y;
  };

  invert_y(target_stroke.p0);
  invert_y(target_stroke.p1);
  invert_y(target_stroke.p2);
}