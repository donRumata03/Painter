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

inline void test_stroke_derivative_counting() {
	// Comparing following derivative and simple counting:
	// TODO!!
}
