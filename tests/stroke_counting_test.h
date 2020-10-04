//
// Created by Vova on 25.07.2020.
//

#pragma once

#include "data_representation/stroke.h"

inline void test_stroke_counting()
{
	auto stroke1 = stroke{
			{ 110, 110 }, { 200, 500 }, { 1000, 800 }, 100
	};

	for (auto t : linspace(0, 1, 10, true)) {
		std::cout << t << " : " << stroke1.coords_at(t) << std::endl;
	}
}

inline void test_stroke_for_each() {
	auto limits = RangeRectangle<lint> {
		0, 10, 0, 10
	};

	stroke stroke{
			{},
			{},
			{}
	};

	std::cout << stroke.get_points(10000, limits) << std::endl;
}

inline void test_stroke_inverse_counting() {
	stroke stroke { { 5, 1 }, { 6, 7 }, { 0, 10 }, 1 };

	point c1 = stroke.coords_at(0.5);

	std::cout << "Midpoint: " << c1 << std::endl;

	// stroke.t_at(c1);
	// std::cout << stroke.t_at( stroke.coords_at(0.1) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.2) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.3) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.4) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.5) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.6) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.7) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.8) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(0.9) ) << std::endl;
	// std::cout << stroke.t_at( stroke.coords_at(1) )   << std::endl;

	std::cout << "_____________________________________________________________________________________" << std::boolalpha << std::endl;

	for (auto t : linspace(0, 1, 100, true)) {
		std::cout << t << " " << stroke.t_at( stroke.coords_at(t)) << " " << almost_equal(stroke.t_at( stroke.coords_at(t) ), t) << std::endl;
	}
}

std::vector<point> get_stroke_path_by_derivative(const stroke& stroke, size_t n)
{
	std::vector<point> res;
	res.reserve(n + 1);

	point accumulator = stroke.coords_at(0);
	double step = 1. / n;

	res.push_back(accumulator);

	for (size_t index = 1; index <= n; ++index) {
		double t = index / double(n);

		accumulator = accumulator + step * stroke.derivative_at(t);
		res.push_back(accumulator);
	}

	return res;
}

inline void test_stroke_derivative_counting() {
	// Comparing following derivative and simple counting:

	size_t n = 2000;
	double step = 1. / n;

	stroke test_stroke { { 5, 1 }, { 6, 7 }, { 0, 10 }, 1 };
	// point accumulator = test_stroke.coords_at(0);

	std::vector<point> true_path;
	true_path.reserve(n + 1);
	true_path.push_back(test_stroke.coords_at(0));

	for (size_t index = 1; index <= n; ++index) {
		double t = index / double(n);

		point real_value = test_stroke.coords_at(t);
		// accumulator = accumulator + step * test_stroke.derivative_at(t);

		true_path.push_back(real_value);
		// std::cout << "Required: " << real_value << "Have using derivative: " << accumulator << std::endl;
	}

	auto precise_derivative_path = get_stroke_path_by_derivative(test_stroke, 200);
	auto non_precise_derivative_path = get_stroke_path_by_derivative(test_stroke, 20);

	add_points_to_plot(true_path, { .name = "Real path" });
	add_points_to_plot(precise_derivative_path, { .name = "Precise path" });
	add_points_to_plot(non_precise_derivative_path, { .name = "Not precise path" });

	show_plot();
}
