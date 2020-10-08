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

inline std::vector<point> get_stroke_path_by_derivative(const stroke& stroke, size_t n)
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

inline double get_stroke_length_by_derivative(const stroke& stroke, size_t n)
{
	double res = 0;

	point accumulator = stroke.coords_at(0);
	double step = 1. / n;

	for (size_t index = 1; index <= n; ++index) {
		double t = index / double(n);

		res += step * abs(stroke.derivative_at(t));
	}

	return res;
}

inline double get_stroke_length_by_counting_path(const stroke& stroke, size_t n)
{
	double res = 0;

	point prev_point = stroke.coords_at(0);
	double step = 1. / n;

	for (size_t index = 1; index <= n; ++index) {
		double t = index / double(n);
		point new_point = stroke.coords_at(t);
		res += point::dist(new_point, prev_point);

		prev_point = new_point;
	}

	return res;
}




inline void test_length_function_by_comparing_with_derivative() {
	size_t n = 2000;
	double step = 1. / n;

	stroke test_stroke { { 5, 1 }, { 6, 7 }, { 0, 10 }, 1 };

	double deriv_length = get_stroke_length_by_derivative(test_stroke, 100'000);
	double path_length = get_stroke_length_by_counting_path(test_stroke, 100'000);

	double analitic_length = test_stroke.length();

	std::cout << "Length by derivative: " << deriv_length << std::endl;
	std::cout << "Length by path: " << path_length << std::endl;
	std::cout << "Length analitically: " << analitic_length << std::endl;
}


inline void test_stroke_scaling() {
	auto image = get_default_image();

	std::vector<std::pair<double, double>> points = {
			{ 110, 110 }, { 200, 500 }, { 1000, 800 }
	};

	auto test_stroke = colored_stroke{ points[0], points[1], points[2],
	                                             100, {1., 1., 1}};

	rasterize_stroke(image, test_stroke, 10000);
	show_image_in_system_viewer(image);

	test_stroke.scale_from_center(0.5);
	test_stroke.background_color = { 1, 0.1, 0.1 };

	rasterize_stroke(image, test_stroke, 10000);
	show_image_in_system_viewer(image);
}
