//
// Created by Vova on 16.07.2020.
//

#include "stroke_rasterizer.h"

void rasterize_stroke (Image &target_image, const stroke &stroke, size_t point_number)
{
	for (size_t point_index = 0; point_index < point_number; ++point_index) {

	}
}

void rasterize_strokes (Image &target_image, const std::vector<stroke> &strokes)
{
	for (const auto& stroke : strokes) rasterize_stroke(target_image, stroke);
}


