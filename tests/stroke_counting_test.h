//
// Created by Vova on 25.07.2020.
//

#pragma once

#include "utils/stroke.h"

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


