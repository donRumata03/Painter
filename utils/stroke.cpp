//
// Created by Vova on 13.07.2020.
//

#include "stroke.h"

colored_stroke::colored_stroke (const point &point1, const point &point2, const point &point3, double _width, color _color)
			: stroke{point1, point2, point3, _width},
			  background_color(_color)
{}


stroke::point stroke::coords_at (double t) const
{
	assert(t >= 0 && t <= 1);

	point result;
	for (size_t coord_index = 0; coord_index < 2; ++coord_index) {
		result[coord_index] =
				p1[coord_index]
				+ square(1 - t) * (p0[coord_index] - p1[coord_index])
				+ square(t) * (p2[coord_index] - p1[coord_index]);
	}

	return result;
}

double stroke::height_at (double t) const
{
	assert(t >= 0 && t <= 1);

	// TODO?

	return width;
}

std::vector<point> stroke::get_points (
		size_t step_number, std::optional<Range_rectangle<lint>> range_limits) const
{
	std::vector<point> res;

	for_each(
	[&](lint x, lint y){
		res.emplace_back( double(x), double(y) );
	},
			step_number, range_limits);

	return res;
}















/*

template < class Functor >
void stroke::for_each (const Functor &operation, size_t step_number,
                       std::optional<Range_rectangle<lint>> range_limits) const
{
	const_cast<colored_stroke*>(this)->for_each(operation, step_number, range_limits);
}
*/
