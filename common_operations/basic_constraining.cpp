//
// Created by Vova on 29.09.2020.
//

#include "basic_constraining.h"


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

double get_best_shift(const std::pair<double, double>& target_coords, const std::pair<double, double>& rect_coords, double randomness_coefficient = 0.1) {
	assert(target_coords.second > target_coords.first);
	assert(rect_coords.second > rect_coords.first);

	// TODO: add some randomness here to prevent concentrating many strokes on the border
	// double sigma = (rect_coords.second - rect_coords.first) * randomness_coefficient;

	// double movement_factor = randomness_coefficient + 1;
	// double res = 0;
	// return std::clamp(res * movement_factor, rect_coords.first, rect_coords.second);

	double delta = 0;

	if (target_coords.first < rect_coords.first) {
		// Move up
		delta = rect_coords.first - target_coords.first;
	}
	else {
		// Move down
		delta = rect_coords.second - target_coords.second;
	}

	return delta;
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

	brute_constrain_stroke_to_fit_into_rect(stroke, rectangle);
}

