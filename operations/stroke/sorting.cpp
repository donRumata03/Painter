#include "sorting.h"
#include "utils/logger.h"


size_t get_default_number_of_zones_by_stroke_number(size_t stroke_number) {
  // the number of sectors = zones^2, how much strokes should be there in the sector?
  constexpr double strokes_in_sector_to_all_strokes_fraction = 0.04;
  constexpr double typical_strokes_in_sector = 4.;
  constexpr size_t minimal_zones = 3;


  double strokes_in_sector = geometric_mean(
          {typical_strokes_in_sector, double(stroke_number) * strokes_in_sector_to_all_strokes_fraction});

  double sectors = double(stroke_number) / strokes_in_sector;
  return std::max(minimal_zones, size_t(std::round(std::sqrt(sectors))));
}


/**
 * Sorts strokes the way that strokes with close indexes are close on each other on the image plane
 * (i.e. tries to minimize resultant distance passed by the brush)
 * TODO: think about invariant of preserving resultant image (not to change order of overlapping strokes)
 */
template <class StrokeType>
std::vector<StrokeType> sort_strokes(const std::vector<StrokeType>& strokes, double w, double h, size_t zones) {
  if (zones == 0) {
    zones = get_default_number_of_zones_by_stroke_number(strokes.size());
  }

  LogInfo("Stroke Sorting") << "Number of zones: " << zones;

  std::vector<StrokeType> res(strokes.size());
  std::copy(strokes.begin(), strokes.end(), res.begin());

  std::sort(res.begin(), res.end(), [&](const StrokeType& s1, const StrokeType& s2) {
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
      return {size_t(std::round(p.y / (h / zones))), size_t(std::round(p.x / (w / zones))),
      };
    };

    return zone_finder(s1.p0) < zone_finder(s2.p0);
  });

  return res;
}

template std::vector<ColoredStroke>
sort_strokes(const std::vector<ColoredStroke>& strokes, double w, double h, size_t zones);

template std::vector<ByteColoredStroke>
sort_strokes(const std::vector<ByteColoredStroke>& strokes, double w, double h, size_t zones);


template <class StrokeType>
std::vector<std::vector<StrokeType>> group_sorted_strokes_by_color(const std::vector<StrokeType>& strokes) {
  std::vector<std::vector<StrokeType>> result;

  for (auto& this_stroke: strokes) {
    if (result.empty() or result.back().back().background_color != this_stroke.background_color) {
      result.emplace_back();
    }
    result.back().push_back(this_stroke);
  }

  return result;
}

template <>
std::vector<std::vector<ColoredStroke>> group_sorted_strokes_by_color(const std::vector<ColoredStroke>& strokes);

template <>
std::vector<std::vector<ByteColoredStroke>>
group_sorted_strokes_by_color(const std::vector<ByteColoredStroke>& strokes);