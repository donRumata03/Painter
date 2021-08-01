//
// Created by Vova on 13.07.2020.
//

#include "stroke.h"

/*

colored_stroke::colored_stroke (const Point &point1, const Point &point2, const Point &point3, double _width, color _color)
			: stroke{point1, point2, point3, _width},
			  background_color(_color)
{}
*/


stroke::Point stroke::coords_at (double t) const
{
	assert(t >= 0 && t <= 1);

/*
	Point result;
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
	Point res = 2 * (t - 1) * (p0 - p1) + 2 * t * (p2 - p1);
	return res;
}



double stroke::t_at (const Point& point_in_stroke) const
{
	auto is_good_t = [](double t_candidate) -> bool { return t_candidate >= 0 and t_candidate <= 1; };

	Point the_result;

	if ((p0 - 2 * p1 + p2).is_zero()) {
		if (p0 != p1) {
			the_result = (point_in_stroke - p0) / (2 * (p1 - p0));
		}
		else { // p0 == p1 != p2
			Point for_sqrt = (point_in_stroke - p0) / (p2 - p1);
			assert(for_sqrt.x != 0 and for_sqrt.y != 0);
			the_result = sqrt(for_sqrt);
		}

		if (is_good_t(the_result.x)) return the_result.x;
		assert(is_good_t(the_result.y));
		return the_result.y;
	}
	else { // p0 - 2p1 + p2 != 0
		Point for_sqrt = (p0 - 2 * p1 + p2) * point_in_stroke + p1 * p1 - p0 * p2;
		assert(for_sqrt.x != 0 and for_sqrt.y != 0);
		Point the_sqr = sqrt(for_sqrt);


		Point solution1 = (p0 - p1 + the_sqr) / (p0 - 2 * p1 + p2);
		Point solution2 = (p0 - p1 - the_sqr) / (p0 - 2 * p1 + p2);

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

	Point a = p0 - 2 * p1 + p2;
	Point b = 2 * (p1 - p0);

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
		size_t step_number, std::optional<RangeRectangle<lint>> range_limits, StrokeRasterizationAlgorithm algo) const
{
	std::vector<Point> res;

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


void stroke::scale_x_from_center (double scale_factor)
{
	double center_x = center().x;

	p0.scale_x_from(center_x, scale_factor);
	p1.scale_x_from(center_x, scale_factor);
	p2.scale_x_from(center_x, scale_factor);
}

void stroke::scale_y_from_center (double scale_factor)
{
	double center_y = center().y;

	p0.scale_y_from(center_y, scale_factor);
	p1.scale_y_from(center_y, scale_factor);
	p2.scale_y_from(center_y, scale_factor);
}

void stroke::scale_from_center (double scale_factor)
{
	Point center = this->center();

	p0.scale_from(center, scale_factor);
	p1.scale_from(center, scale_factor);
	p2.scale_from(center, scale_factor);
}

RangeRectangle<double> stroke::get_curve_bounding_box () const
{
	RangeRectangle<double> res{};

	res.min_x = std::numeric_limits<double>::max();
	res.min_y = std::numeric_limits<double>::max();
	res.max_x = std::numeric_limits<double>::min();
	res.max_y = std::numeric_limits<double>::min();

	auto process_point = [&](const stroke::Point& p) {
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

RangeRectangle<double> stroke::get_stroke_bounding_box () const
{
	RangeRectangle<double> box = get_curve_bounding_box();

	box.min_y -= width;
	box.min_x -= width;

	box.max_y += width;
	box.max_x += width;

	return box;
}


/*

template < class Functor >
void stroke::for_each (const Functor &operation, size_t step_number,
                       std::optional<RangeRectangle<lint>> range_limits) const
{
	const_cast<colored_stroke*>(this)->for_each(operation, step_number, range_limits);
}
*/


/// Json

void to_json(json& j, const stroke& stroke)
{
	j["type"] = "quadratic_bezier";

	j["p0"] = stroke.p0;
	j["p1"] = stroke.p1;
	j["p2"] = stroke.p2;

	j["width"] = stroke.width;
}

void to_json(json& j, const colored_stroke& col_stroke)
{
    to_json(j, (stroke&)col_stroke);

    j["color"] = convert_color<uint8_t>(col_stroke.background_color);
}


void to_json(json& j, const std::vector<colored_stroke>& strokes)
{
    for (size_t i = 0; i < strokes.size(); i++) j[i] = strokes[i];
}

void from_json(const json& j, stroke& stroke)
{
	assert(j["type"] == "quadratic_bezier");

	stroke.p0 = j["p0"].get<point>();
	stroke.p1 = j["p1"].get<point>();
	stroke.p2 = j["p2"].get<point>();

	stroke.width = j["width"].get<double>();
}

void from_json(const json& j, colored_stroke& col_stroke)
{
    assert(not j.at("color").empty());

    from_json(j, (stroke&)col_stroke);
    byte_color col = j.at("color");
    col_stroke.background_color = convert_color<double>(col);
}

void from_json(const json& j, std::vector<colored_stroke>& strokes)
{
    for (size_t i = 0; i < strokes.size(); i++) strokes[i] = j[i];
}