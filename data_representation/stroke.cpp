//
// Created by Vova on 13.07.2020.
//

#include "stroke.h"

/*

colored_stroke::colored_stroke (const point &point1, const point &point2, const point &point3, double _width, color _color)
			: stroke{point1, point2, point3, _width},
			  background_color(_color)
{}
*/


stroke::point stroke::coords_at (double t) const
{
	assert(t >= 0 && t <= 1);

/*
	point result;
	for (size_t coord_index = 0; coord_index < 2; ++coord_index) {
		result[coord_index] =
				p1[coord_index]
				+ square(1 - t) * (p0[coord_index] - p1[coord_index])
				+ square(t) * (p2[coord_index] - p1[coord_index]);
	}

	return result;
*/
	return p1 + square(1 - t) * (p0 - p1) + square(t) * (p2 - p1);
}

double stroke::height_at (double t) const
{
	assert(t >= 0 && t <= 1);

	// TODO?

	return width;
}


point stroke::derivative_at (double t) const
{
	point res = 2 * (t - 1) * (p0 - p1) + 2 * t * (p2 - p1);
	return res;
}



double stroke::t_at (const point& point_in_stroke) const
{
	auto is_good_t = [](double t_candidate) -> bool { return t_candidate >= 0 and t_candidate <= 1; };

	point the_result;

	if ((p0 - 2 * p1 + p2).is_zero()) {
		if (p0 != p1) {
			the_result = (point_in_stroke - p0) / (2 * (p1 - p0));
		}
		else { // p0 == p1 != p2
			point for_sqrt = (point_in_stroke - p0) / (p2 - p1);
			assert(for_sqrt.x != 0 and for_sqrt.y != 0);
			the_result = sqrt(for_sqrt);
		}

		if (is_good_t(the_result.x)) return the_result.x;
		assert(is_good_t(the_result.y));
		return the_result.y;
	}
	else { // p0 - 2p1 + p2 != 0
		point for_sqrt = (p0 - 2 * p1 + p2) * point_in_stroke + p1 * p1 - p0 * p2;
		assert(for_sqrt.x != 0 and for_sqrt.y != 0);
		point the_sqr = sqrt(for_sqrt);


		point solution1 = (p0 - p1 + the_sqr) / (p0 - 2 * p1 + p2);
		point solution2 = (p0 - p1 - the_sqr) / (p0 - 2 * p1 + p2);

		// std::cout << "Solutions: " << solution1 << " " << solution2 << std::endl;

		std::vector<double> solutions = { solution1.x, solution1.y, solution2.x, solution2.y };
		std::sort(solutions.begin(), solutions.end());
		// std::cout << solutions << std::endl;

		size_t solution_index = 0;
		for (auto& possible_t : solutions) {
			// std::cout << possible_t << " " << (possible_t >= 0) << std::endl;
			if (solution_index != solutions.size() - 1 and is_good_t(possible_t) and almost_equal(possible_t, solutions[solution_index + 1], 1e-7)) {
				return possible_t;
			}
			solution_index++;
		}

		// If didn't find 2 appropriate solutions, try looking for only one:
		for(auto& possible_t : solutions) {
			if(is_good_t(possible_t)) {
				return possible_t;
			}
		}

		assert(false);
	}


	return -1;
}

double stroke::length () const
{
	// For formula derivation visit https://malczak.linuxpl.com/blog/quadratic-bezier-curve-length/

	point a = p0 - 2 * p1 + p2;
	point b = 2 * (p1 - p0);

	double A = 4 * squared_abs(a);
	double B = 4 * (a.x * b.x + a.y * b.y);
	double C = squared_abs(b);

	double S_abc = 2 * sqrt(A + B + C);
	double A_2 = sqrt(A);
	double A_32 = 2 * A * A_2;
	double C_2 = 2 * sqrt(C);
	double BA = B / A_2;

	return ( A_32 * S_abc +
	         A_2*B*(S_abc-C_2) +
	         (4 * C * A - B * B)*log((2 * A_2 + BA + S_abc)/(BA + C_2))
	       )
	       /
	       (4*A_32);
}



std::vector<point> stroke::get_points (
		size_t step_number, std::optional<RangeRectangle<lint>> range_limits) const
{
	std::vector<point> res;

	this->for_each(
	[&](lint x, lint y){
		res.emplace_back( double(x), double(y) );
	},
			step_number, range_limits);

	return res;
}

std::ostream &operator<< (std::ostream &os, const stroke &stroke)
{
	os << "Stroke { p0: " << stroke.p0 << " p1: " << stroke.p1 << " p2: " << stroke.p2 << " width: " << stroke.width << " }";
	return os;
}


void stroke::scale_from_center (double scale_factor)
{
	point center = this->center();

	p0.scale_from(center, scale_factor);
	p1.scale_from(center, scale_factor);
	p2.scale_from(center, scale_factor);
}

RangeRectangle<double> stroke::get_bounding_box () const
{
	RangeRectangle<double> res{};

	res.min_x = std::numeric_limits<double>::max();
	res.min_y = std::numeric_limits<double>::max();
	res.max_x = std::numeric_limits<double>::min();
	res.max_y = std::numeric_limits<double>::min();

	auto process_point = [&](const stroke::point& p) {
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



/*

template < class Functor >
void stroke::for_each (const Functor &operation, size_t step_number,
                       std::optional<RangeRectangle<lint>> range_limits) const
{
	const_cast<colored_stroke*>(this)->for_each(operation, step_number, range_limits);
}
*/
std::ostream &operator<< (std::ostream &os, const colored_stroke &stroke)
{
	os << reinterpret_cast<const struct stroke &>(stroke) << " background_color: " << stroke.background_color;
	return os;
}
