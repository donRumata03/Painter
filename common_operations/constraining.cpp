//
// Created by Vova on 29.09.2020.
//

#include "constraining.h"


bool fits_into (const stroke &stroke, const RangeRectangle<double> &rectangle)
{
	return rectangle.point_satisfies_requirements(stroke.p0) and
		   rectangle.point_satisfies_requirements(stroke.p1) and
		   rectangle.point_satisfies_requirements(stroke.p2);
}

void shift_point (stroke::point& point, const stroke::point& shifting_vector) {
	point.x += shifting_vector.x;
	point.y += shifting_vector.y;
}

void shift_stroke (stroke &stroke, const stroke::point &shifting_vector)
{
	shift_point(stroke.p0, shifting_vector);
	shift_point(stroke.p1, shifting_vector);
	shift_point(stroke.p2, shifting_vector);
}

double get_best_shift(const stroke::point& target_coords, const stroke::point& rect_coords) {
	// TODO
}

////////////////////////////////////////

void brute_constrain_stroke_to_fit_into_rect (stroke &stroke, const RangeRectangle<double> &rectangle)
{
	rectangle.constrain_point(stroke.p0);
	rectangle.constrain_point(stroke.p1);
	rectangle.constrain_point(stroke.p2);
}

void carefully_constrain_stroke_to_fit_into_rect (stroke &stroke, const RangeRectangle<double> &rectangle)
{
	if (fits_into(stroke, rectangle)) return;

	// Move it (x and y independently)

	auto stroke_rectangle = stroke.get_bounding_box();

	double dx = get_best_shift( { stroke_rectangle.min_x, stroke_rectangle.max_x }, { rectangle.min_x, rectangle.max_x } );
	double dy = get_best_shift( { stroke_rectangle.min_y, stroke_rectangle.max_y }, { rectangle.min_y, rectangle.max_y } );

	shift_stroke(stroke, { dx, dy });
}

