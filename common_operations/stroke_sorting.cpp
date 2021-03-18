//
// Created by Vova on 06.03.2021.
//

#include "stroke_sorting.h"


std::vector<byte_colored_stroke> sort_strokes (const std::vector<byte_colored_stroke>& strokes, double w, double h, size_t zones)
{
	std::vector<byte_colored_stroke> res(strokes.size());
	std::copy(strokes.begin(), strokes.end(), res.begin());

	std::sort(res.begin(), res.end(), [&] (const byte_colored_stroke& s1, const byte_colored_stroke& s2){
		// By color:
		if (s1.background_color != s2.background_color) {
			return s1.background_color < s2.background_color;
		}

		// Otherwise: by coordinates.
		// Divide the image by zones because the coordinates are real-valued, so the the algorithm would become indifferent to y-axis.

		// sort
		// 1. by x_zone
		// 2. by y_zone
		auto zone_finder = [&](point p) -> std::pair<size_t, size_t> {
			return {
				size_t(std::round( p.y / (h / zones) )),
				size_t(std::round( p.x / (w / zones) )),
			};
		};

		return zone_finder(s1.p0) < zone_finder(s2.p0);
	});

	return res;
}


std::vector<std::vector<byte_colored_stroke>> group_sorted_strokes_by_color (const std::vector<byte_colored_stroke>& strokes)
{
	std::vector<std::vector<byte_colored_stroke>> res;

	for (auto& this_stroke: strokes) {
		if (res.empty() or res.back().back().background_color != this_stroke.background_color) {
			res.emplace_back();
		}
		res.back().push_back(this_stroke);
	}

	return res;
}
