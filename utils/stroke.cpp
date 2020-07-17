//
// Created by Vova on 13.07.2020.
//

#include "stroke.h"

colored_stroke::colored_stroke (const point &point1, const point &point2, const point &point3, double _width, color _color)
			: stroke{point1, point2, point3, _width},
			  background_color(_color)
{}


colored_stroke::point stroke::coords_at (double t) const
{
	assert(t >= 0 && t <= 1);



	return {  };
}

double stroke::height_at (double t) const
{
	assert(t >= 0 && t <= 1);

	// TODO

	return width;
}

std::vector<point> stroke::get_points (
		size_t step_number, std::optional<Range_rectangle<lint>> range_limits) const
{
	std::vector<point> res;

	for_each(
	[&](size_t x, size_t y){
		res.emplace_back( x, y );
	},
			step_number, range_limits);

	return res;
}

template < class Functor >
void stroke::for_each (
		const Functor& operation, const size_t step_number, std::optional<Range_rectangle<lint>> range_limits) const
{
	bool has_range_limitations = bool(range_limits);
	auto last_x = static_cast<long long>(-1);

	for (size_t point_index = 0; point_index < step_number; ++point_index) {
		double t = double(point_index) / step_number;

		auto [central_x, central_y] = coords_at(t);
		auto x = long long(std::round(central_x));

		if (x == last_x) continue; // To avoid repetitions
		last_x = x;

		if (has_range_limitations && x < range_limits->min_x || x >= range_limits->max_x) {
			continue; // To satisfy range
		}

		double height = height_at(t);
		double height_half = height / 2;

		auto y0 = size_t(std::round(central_y - height_half));
		auto y1 = size_t(std::round(y0 + height));

		if (has_range_limitations) {
			y0 = std::clamp(y0, range_limits->min_y, range_limits->max_y - 1);
			y1 = std::clamp(y1, range_limits->min_y, range_limits->max_y - 1);
		}

		// Vertical line here:
		for (size_t y = y0; y < y1; ++y) {
			operation(x, y);
		}
	}
}


/*

template < class Functor >
void stroke::for_each (const Functor &operation, size_t step_number,
                       std::optional<Range_rectangle<lint>> range_limits) const
{
	const_cast<colored_stroke*>(this)->for_each(operation, step_number, range_limits);
}
*/
