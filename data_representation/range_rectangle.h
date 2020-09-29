//
// Created by Vova on 29.09.2020.
//

#pragma once

#include "painter_pch.h"

template<class T>
struct RangeRectangle {
	T min_x, max_x;
	T min_y, max_y;

	bool point_satisfies_requirements(const point& point) {
		return (point.x >= min_x and point.x <= max_x) and (point.y >= min_y and point.y <= max_y);
	}

	void constrain_point(point& point) {
		point.x = std::clamp(point.x, double(min_x), double(max_x));
		point.y = std::clamp(point.x, double(min_x), double(max_x));
	}

	point get_center () {
		return {
				(min_x + max_x) / 2,
				(min_y + max_y) / 2,
		};
	}
};

inline RangeRectangle<lint> get_image_range_limits(const Image& image) {
	return {
			0, image.cols, 0, image.rows
	};
}

