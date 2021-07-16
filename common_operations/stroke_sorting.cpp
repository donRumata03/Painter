//
// Created by Vova on 06.03.2021.
//

#include "stroke_sorting.h"
#include "utils/Logger.h"


size_t get_default_number_of_zones_by_stroke_number (size_t stroke_number)
{
	// the number of sectors = zones^2, how much strokes should be there in the sector?
	constexpr double strokes_in_sector_to_all_strokes_fraction = 0.04;
	constexpr double typical_strokes_in_sector = 4.;
	constexpr size_t minimal_zones = 3;


	double strokes_in_sector = geometric_mean({
			                                          typical_strokes_in_sector,
			                                          double(stroke_number) * strokes_in_sector_to_all_strokes_fraction
	                                          });
	// std::cout << "strokes_in_sector: " << strokes_in_sector << std::endl;

	double sectors = double(stroke_number) / strokes_in_sector;
	return std::max(minimal_zones, size_t(std::round(std::sqrt(sectors))));
}



template<class StrokeType>
std::vector<StrokeType> sort_strokes (const std::vector<StrokeType>& strokes, double w, double h, size_t zones)
{
	if (zones == 0) {
		zones = get_default_number_of_zones_by_stroke_number(strokes.size());
	}

    LogInfo("Stroke Sorting") << "Number of zones: " << zones;

	std::vector<StrokeType> res(strokes.size());
	std::copy(strokes.begin(), strokes.end(), res.begin());

	std::sort(res.begin(), res.end(), [&] (const StrokeType& s1, const StrokeType& s2){
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

template std::vector<colored_stroke> sort_strokes (const std::vector<colored_stroke>& strokes, double w, double h, size_t zones);
template std::vector<byte_colored_stroke> sort_strokes (const std::vector<byte_colored_stroke>& strokes, double w, double h, size_t zones);


template<class StrokeType>
std::vector<std::vector<StrokeType>> group_sorted_strokes_by_color (const std::vector<StrokeType>& strokes)
{
	std::vector<std::vector<StrokeType>> res;

	for (auto& this_stroke: strokes) {
		if (res.empty() or res.back().back().background_color != this_stroke.background_color) {
			res.emplace_back();
		}
		res.back().push_back(this_stroke);
	}

	return res;
}

template std::vector<std::vector<colored_stroke>> group_sorted_strokes_by_color (const std::vector<colored_stroke>& strokes);
template std::vector<std::vector<byte_colored_stroke>> group_sorted_strokes_by_color (const std::vector<byte_colored_stroke>& strokes);