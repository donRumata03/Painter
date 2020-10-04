//
// Created by Vova on 13.07.2020.
//

#ifndef PAINTER_STROKE_H
#define PAINTER_STROKE_H

#include <pythonic.h>
#include <ostream>

#include "color.h"
#include "range_rectangle.h"


/**
 * It`s basically a 2nd order Bezier curve => contains 3x 2d-points
*/
struct stroke {
	// using point = std::pair<double, double>;
	using point = point;

	point p0{}, p1{}, p2{};
	double width = -1;

	// stroke() = default;
	// colorless_stroke(
	// 		const point& point1, const point& point2, const point& point3, double width) = default;

	[[nodiscard]] point coords_at(double t) const; // Count the point of Bezier curve corresponding to t value given as the argument
	[[nodiscard]] double height_at(double t) const; // The height of the curve at x corresponding to time point t
	[[nodiscard]] point derivative_at(double t) const; // Returns pair of dx/dt and dy/dt
	[[nodiscard]] double t_at(const point& point_in_stroke) const;
	[[nodiscard]] double length() const;

	/**
	 * Width isn`t used to determine the rectangle
	 */
	[[nodiscard]] RangeRectangle<double> get_bounding_box() const;

	/**
	 * @param range_limits: if it`s not std::nullopt, only pixels
	 * for x in [range_params->min_x, range_params->max_x)
	 * and for y in [range_params->min_y, range_params->max_y)
	 * are processed.
	*/
	template<class Functor>
	void for_each(const Functor& operation, size_t step_number = 10000,
	              std::optional<RangeRectangle<lint>> range_limits = std::nullopt) const;
	// ^^^ TODO: Measure and ...make it parallel?


	[[nodiscard]] std::vector<point> get_points(
			size_t step_number = 10000, std::optional<RangeRectangle<lint>> range_limits = std::nullopt
	) const;

	friend std::ostream &operator<< (std::ostream &os, const stroke &stroke);
};


/// Template function implementations:

template < class Functor >
void stroke::for_each (
		const Functor& operation, const size_t step_number, std::optional<RangeRectangle<lint>> range_limits) const
{
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
struct colored_stroke : stroke
{
	color background_color;

	friend std::ostream &operator<< (std::ostream &os, const colored_stroke &stroke);

	/*
	colored_stroke() = default;
	colored_stroke(const point &point1, const point &point2, const point &point3, double _width, color _color);
	colored_stroke(const stroke& colorless_stroke, const color& bg_color) :
		stroke(colorless_stroke), background_color(bg_color) {}
	*/
};


#endif //PAINTER_STROKE_H
