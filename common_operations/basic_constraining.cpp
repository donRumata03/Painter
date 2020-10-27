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

void shift_point (stroke::Point& point, const stroke::Point& shifting_vector) {
	point.x += shifting_vector.x;
	point.y += shifting_vector.y;
}

void shift_stroke (stroke &stroke, const stroke::Point &shifting_vector)
{
	shift_point(stroke.p0, shifting_vector);
	shift_point(stroke.p1, shifting_vector);
	shift_point(stroke.p2, shifting_vector);
}

double get_best_shift(const std::pair<double, double>& target_coords, const std::pair<double, double>& rect_coords, double randomness_coefficient = 0.1) {
	// assert(target_coords.second > target_coords.first);
	// assert(rect_coords.second > rect_coords.first);

	if (target_coords.second <= target_coords.first || rect_coords.second <= rect_coords.first) {
		std::cout << console_colors::red << "Something's wrong with rect or target coords: " << console_colors::remove_all_colors << std::endl;
		std::cout << "Target coords: " << target_coords << std::endl;
		std::cout << "Rect coords: " << rect_coords << std::endl;
		assert(false);
	}

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

///////////////////////////////////////////////////////////////////////////////////////////////////

void constrain_stroke_size_parameters (stroke &stroke,
                                       double min_dx, double max_dx,
                                       double min_dy, double max_dy,
                                       double min_width, double max_width)
{
	auto bounding_box = stroke.get_bounding_box();

	auto stroke_dx = bounding_box.max_x - bounding_box.min_x;
	auto stroke_dy = bounding_box.max_y - bounding_box.min_y;

	if(stroke_dx < min_dx) stroke.scale_x_from_center(min_dx / stroke_dx);
	if(stroke_dx > max_dx) stroke.scale_x_from_center(max_dx / stroke_dx);

	if(stroke_dy < min_dy) stroke.scale_y_from_center(min_dy / stroke_dy);
	if(stroke_dy > max_dy) stroke.scale_y_from_center(max_dy / stroke_dy);

			                 // Width
	stroke.width = std::clamp(stroke.width, min_width, max_width);
}

