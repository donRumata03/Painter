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
