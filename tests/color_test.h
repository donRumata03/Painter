//
// Created by Vova on 15.07.2020.
//

#pragma once

#include "utils/color.h"

inline void color_hash_test() {
	color c = { 1., 3., 5. };

	std::unordered_set<color> set = {
			{ 1, 3, 4 }
	};

	 // std::cout << set << std::endl;
}

inline void test_color_converting_to_floating_point() {
	byte_color color = {
			255,
			100,
			1
	};
	auto res = to_floating_point<double, uint8_t>(color);
	std::cout << res << std::endl;


	rgb_color<signed char> signed_color = { -100, 100, 0};
	auto res2 = to_floating_point<double, int8_t>(signed_color);
	std::cout << res2 << std::endl;
}

inline void test_color_converting_from_floating_point() {
	color color = {
			1, 0.392157, 0.00392157
	};
	auto res = from_floating_point<uint8_t, double>(color);
	std::cout << res << std::endl; // Should be { 255, 100, 1 }


	rgb_color<double> other_color = { 0.109804, 0.894118, 0.501961 };
	auto signed_color = from_floating_point<int8_t, double>(other_color);
	std::cout << signed_color << std::endl; // Should be { -100, 100, 0 }
}
