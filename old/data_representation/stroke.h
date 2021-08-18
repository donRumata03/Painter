//
// Created by Vova on 13.07.2020.
//

#ifndef PAINTER_STROKE_H
#define PAINTER_STROKE_H

#include <pythonic.h>
#include <ostream>

#include "color.h"
#include "canvas.h"
#include "range_rectangle.h"



enum class StrokeRasterizationAlgorithm
{
	vertical_lines,
	horizontal_lines,
	vertical_and_horizontal_lines,
	smooth
};


/**
 * It`s basically a 2nd order Bezier curve => contains 3 × 2d-points
*/
struct stroke {
	// using Point = std::pair<double, double>;
	using Point = point;

	Point p0{}, p1{}, p2{};
	double width = -1;

	// stroke() = default;
	// colorless_stroke(
	// 		const Point& point1, const Point& point2, const Point& point3, double width) = default;

	[[nodiscard]] Point coords_at(double t) const; // Count the Point of Bezier curve corresponding to t value given as the argument
	[[nodiscard]] double height_at(double t) const; // The height of the curve at x corresponding to time Point t
	[[nodiscard]] Point derivative_at(double t) const; // Returns pair of dx/dt and dy/dt
	[[nodiscard]] double t_at(const Point& point_in_stroke) const;
	[[nodiscard]] double length() const;

	/**
	 * Width isn`t used to determine the rectangle
	 */
	[[nodiscard]] RangeRectangle<double> get_curve_bounding_box() const;
	[[nodiscard]] RangeRectangle<double> get_stroke_bounding_box() const;
	[[nodiscard]] Point center() const { return (p0 + p1 + p2) / 3; }
	void scale_x_from_center(double scale_factor);
	void scale_y_from_center(double scale_factor);
	void scale_from_center(double scale_factor);

	/**
	 * @param range_limits: if it`s not std::nullopt, only pixels
	 * for x in [range_params->min_x, range_params->max_x)
	 * and for y in [range_params->min_y, range_params->max_y)
	 * are processed.
	*/
	template<class Functor>
	void for_each(const Functor& operation, size_t step_number = 10000,
	              std::optional<RangeRectangle<lint>> range_limits = std::nullopt,
	              StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::vertical_lines
	) const;
	// ^^^ TODO: Measure and ...make it parallel?


	[[nodiscard]] std::vector<Point> get_points(
			size_t step_number = 10000, std::optional<RangeRectangle<lint>> range_limits = std::nullopt,
			StrokeRasterizationAlgorithm algo = StrokeRasterizationAlgorithm::vertical_lines
	) const;

	friend std::ostream &operator<< (std::ostream &os, const stroke &stroke);
};

/// Template function implementations:

template < class Functor >
void stroke::for_each (
		const Functor& operation, const size_t step_number, std::optional<RangeRectangle<lint>> range_limits,
		StrokeRasterizationAlgorithm algo) const
{
	if (algo != StrokeRasterizationAlgorithm::vertical_lines) throw std::logic_error("this rasterization algorithm is not implemented");

	bool has_range_limitations = bool(range_limits);
	auto last_x = static_cast<long long>(-1e100);

	for (size_t point_index = 0; point_index < step_number; ++point_index) {
		double t = double(point_index) / step_number;

		auto [central_x, central_y] = coords_at(t);
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
 * Colored version of stroke:
 */

template<class T>
struct rgb_colored_stroke : stroke
{
	using ColorDataType = T;

	rgb_color<T> background_color;

	template<class E>
	friend std::ostream &operator<< (std::ostream &os, const rgb_colored_stroke<E> &stroke);

	template<class NewColorType, std::enable_if_t<not std::is_same_v<ColorDataType, NewColorType>, void*> = nullptr>
	explicit operator rgb_colored_stroke<NewColorType> () const;
};

using colored_stroke = rgb_colored_stroke<double>;
using byte_colored_stroke = rgb_colored_stroke<uint8_t>;


template<class T>
std::ostream &operator<< (std::ostream &os, const rgb_colored_stroke<T> &stroke)
{
	os << reinterpret_cast<const struct stroke &>(stroke) << " background_color: " << stroke.background_color;
	return os;
}

template <class T>
template <class NewColorType, std::enable_if_t<not std::is_same_v<T, NewColorType>, void*>>
rgb_colored_stroke<T>::operator rgb_colored_stroke<NewColorType>  () const
{
	using ThisType = rgb_colored_stroke<T>;
	using ResType = rgb_colored_stroke<NewColorType>;

	ResType res;
	*reinterpret_cast<stroke*>(&res) = *reinterpret_cast<const stroke*>(this);
	res.background_color = convert_color<NewColorType>(this->background_color);

	return res;
}


/// For json:

template<class Type>
struct ContextWrapper
{
	Type object;
	const Canvas& canvas;
};

void to_json(json& j, const ContextWrapper<stroke>& stroke_with_size);
void to_json(json& j, const ContextWrapper<colored_stroke>& col_stroke);
void to_json(json& j, const ContextWrapper<std::vector<colored_stroke>>& strokes);

 void from_json(const json& j, stroke& target_stroke, const Canvas& image_size);
// void from_json(const json& j, colored_stroke& col_stroke);
// void from_json(const json& j, std::vector<colored_stroke>& strokes);

#endif //PAINTER_STROKE_H
