#pragma once

#include <pythonic.h>

#include "data/color.h"
#include "data/canvas.h"
#include "data/range_rectangle.h"


enum class StrokeRasterizationAlgorithm { // TODO: move to /rasterization/
  vertical_lines,
  horizontal_lines,
  vertical_and_horizontal_lines,
  smooth
};


/**
 * A quadratic Bezier curve with thickness
 */
struct Stroke {
  Point p0{}, p1{}, p2{};
  double width;

  Stroke() = default;

  Stroke(Point p0, Point p1, Point p2, double width = 1) : p0(p0), p1(p1), p2(p2), width(width) {}

  /**
   * Count the Point of Bezier curve corresponding to t value given as the argument
   */
  [[nodiscard]] Point coords_at(double t) const;

  /**
   * The height of the curve at x corresponding to time Point t
   */
  [[nodiscard]] double height_at(double t) const;

  /**
   * @return A pair of dx/dt and dy/dt
   */
  [[nodiscard]] Point derivative_at(double t) const;
  [[nodiscard]] double t_at(const Point& point_in_stroke) const;
  [[nodiscard]] double length() const;

  [[nodiscard]] RangeRectangle<double> get_curve_bounding_box() const;
  [[nodiscard]] RangeRectangle<double> get_stroke_bounding_box() const;
  [[nodiscard]] Point center() const { return (p0 + p1 + p2) / 3; }
  void scale_x_from_center(double scale_factor);
  void scale_y_from_center(double scale_factor);
  void scale_from_center(double scale_factor);

  /**
   * @param range_limits If it`s not std::nullopt, only pixels for x in [range_params->min_x, range_params->max_x) and
   * for y in [range_params->min_y, range_params->max_y) are processed.
  */
  template <class Functor>
  void for_each(const Functor& operation, size_t step_number = 10000,
                std::optional<RangeRectangle<lint>> range_limits = std::nullopt,
                StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::vertical_lines) const;
  // TODO: Measure and make it parallel?


  [[nodiscard]] std::vector<Point> get_points(
          size_t step_number = 10000,
          std::optional<RangeRectangle<lint>> range_limits = std::nullopt,
          StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::vertical_lines) const;

  friend std::ostream& operator<<(std::ostream& os, const Stroke& stroke);
  auto operator<=>(const Stroke& stroke) const = default;
};


/// Template function implementations

template <class Functor>
void Stroke::for_each(const Functor& operation, const size_t step_number,
                      std::optional<RangeRectangle<lint>> range_limits, StrokeRasterizationAlgorithm algo) const {
  if (algo != StrokeRasterizationAlgorithm::vertical_lines)
    throw std::logic_error("This rasterization algorithm is not implemented");

  bool has_range_limitations = bool(range_limits);
  auto last_x = static_cast<long long>(-1e100);

  for (size_t point_index = 0; point_index < step_number; ++point_index) {
    double t = double(point_index) / step_number;

    auto[central_x, central_y] = coords_at(t);
    auto x = lint(std::round(central_x));

    if (x == last_x) continue; // To avoid repetitions
    last_x = x;

    if (has_range_limitations && x < range_limits->min_x || x >= range_limits->max_x) {
      continue; // To satisfy range
    }

    double height = height_at(t);
    double height_half = height / 2;

    auto y0 = lint(std::round(central_y - height_half));
    auto y1 = lint(std::round(y0 + height));

    if (has_range_limitations) {
      y0 = std::clamp(y0, range_limits->min_y, range_limits->max_y - 1);
      y1 = std::clamp(y1, range_limits->min_y, range_limits->max_y - 1);
    }

    // Vertical line here:
    for (lint y = y0; y < y1; ++y) {
      operation(x, y);
    }
  }
}


/**
 * Colored version of stroke
 */
template <class T>
struct RgbColoredStroke : Stroke {
  using ColorDataType = T;

  RgbColor<T> background_color;

  RgbColoredStroke() = default;

  RgbColoredStroke(Point p0, Point p1, Point p2, double width = 1, RgbColor<T> color = {0, 0, 0}) : Stroke(p0, p1, p2,
                                                                                                           width),
                                                                                                    background_color(
                                                                                                            color) {}

  RgbColoredStroke(const Stroke& stroke, RgbColor<T> color = {0, 0, 0}) : Stroke(stroke.p0, stroke.p1, stroke.p2,
                                                                                 stroke.width),
                                                                          background_color(color) {}

  template <class E>
  friend std::ostream& operator<<(std::ostream& os, const RgbColoredStroke<E>& stroke);

  template <class NewColorType, std::enable_if_t<not std::is_same_v<ColorDataType, NewColorType>, void *> = nullptr>
  explicit operator RgbColoredStroke<NewColorType>() const;

  auto operator<=>(const RgbColoredStroke& stroke) const = default;
};

using ColoredStroke = RgbColoredStroke<double>;
using ByteColoredStroke = RgbColoredStroke<uint8_t>;


template <class T>
std::ostream& operator<<(std::ostream& os, const RgbColoredStroke<T>& stroke) {
  os << reinterpret_cast<const struct Stroke&>(stroke) << " background_color: " << stroke.background_color;
  return os;
}

template <class OldColorType>
template <class NewColorType, std::enable_if_t<not std::is_same_v<OldColorType, NewColorType>, void *>>
RgbColoredStroke<OldColorType>::operator RgbColoredStroke<NewColorType>() const {
  using OldStrokeType = RgbColoredStroke<OldColorType>;
  using NewStrokeType = RgbColoredStroke<NewColorType>;

  NewStrokeType res;
  *reinterpret_cast<Stroke *>(&res) = *reinterpret_cast<const Stroke *>(this);
  res.background_color = convert_color<NewColorType>(this->background_color);

  return res;
}


/// Json

/**
 * Wrapper for objects e.g. Stroke, combining it with context information about painting
 */
template <class Type>
struct ContextWrapper {
  Type object;
  const Canvas& canvas;
};

void to_json(json& j, const ContextWrapper<Stroke>& stroke_with_size);
void to_json(json& j, const ContextWrapper<ColoredStroke>& col_stroke);
void to_json(json& j, const ContextWrapper<std::vector<ColoredStroke>>& strokes);

void from_json(const json& j, Stroke& target_stroke, const Canvas& canvas);
