//
// Created by Vova on 13.07.2020.
//

#ifndef PAINTER_STROKE_H
#define PAINTER_STROKE_H

#include <pythonic.h>

#include "color.h"

template<class T>
struct Range_rectangle {
	T min_x, max_x;
	T min_y, max_y;
};

/*
 * It`s basically a 2nd order Bezier curve => contains 3x 2d-points
*/
struct stroke
{
	// using point = std::pair<double, double>;
	using point = point;

	point p1, p2, p3;
	double width = -1;
	color background_color;

	stroke() = default;
	stroke(point point1, point point2, point point3, double _width, color _color);
	[[nodiscard]] point coords_at(double t) const; // Count the point of Bezier curve corresponding to t value given as the argument
	[[nodiscard]] double height_at(double t) const; // The height of the curve at x corresponding to time point t

	/**
	 * @param range_limits: if it`s not std::nullopt, only pixels
	 * for x in [range_params->min_x, range_params->max_x)
	 * and for y in [range_params->min_y, range_params->max_y)
	 * are processed.
	 */
	template<class Functor>
	void for_each(const Functor& operation, size_t step_number = 100,
			std::optional<Range_rectangle<lint>> range_limits = std::nullopt);

	template<class Functor>
	void for_each(const Functor& operation, size_t step_number = 100,
	              std::optional<Range_rectangle<lint>> range_limits = std::nullopt) const;


	[[nodiscard]] std::vector<point> get_points(
		size_t step_number = 100, std::optional<Range_rectangle<size_t>> range_limits = std::nullopt
	) const;
};


#endif //PAINTER_STROKE_H
