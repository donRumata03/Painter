#pragma once

#include <utility>

#include "data/stroke_limits.h"
#include "data/common_stroking_params.h"
#include "operations/stroke/limits.h"
#include "operations/stroke/generation.h"
#include "optimization_adapters/utils.h"


class GaGenomeGenerator {
  StrokeLimits limits{};
  size_t strokes_in_image{};

  std::vector<Stroke> basic_strokes;
  bool use_reference = false;

 public:
  GaGenomeGenerator() = default;

  explicit GaGenomeGenerator(const StrokeLimits& limits, size_t stroke_number)
          : limits(limits), strokes_in_image(stroke_number) {}

  explicit GaGenomeGenerator(std::vector<Stroke> strokes) : basic_strokes(std::move(strokes)), use_reference(true) {}

  std::vector<double> operator()(size_t amount) const {
    // Amount isn't the the number of strokes, it's the amount of pieces
    assert(not(amount % (sizeof(Stroke) / sizeof(double))));

    size_t number_of_strokes = amount / 7;
    if (use_reference) {
      assert(number_of_strokes == basic_strokes.size());

      return pack_stroke_data(basic_strokes);
    } else {
      assert(number_of_strokes == strokes_in_image);

      std::vector<Stroke> strokes(strokes_in_image);
      std::generate(strokes.begin(), strokes.end(), [this]() { return generate_stroke(limits); });

      return pack_stroke_data(strokes);
    }
  }
};


std::vector<std::pair<double, double>> generate_point_ranges_for_stroke_genome(
        size_t stroke_number, const point& image_size, const std::pair<double, double>& width_range);

std::vector<double> generate_point_sigmas_for_stroke_genome(size_t stroke_number, const point& image_size,
                                                            double absolute_sigma_for_coords,
                                                            double absolute_sigma_for_width);

CommonStrokingParams switch_to_absolute_values(const CommonStrokingParams& params, size_t w, size_t h);

StrokeLimits generate_stroke_limits_by_raw_parameters(CommonStrokingParams params, size_t w, size_t h);


std::vector<std::pair<double, double>>
generate_point_ranges_by_raw_parameters(CommonStrokingParams params, size_t w, size_t h);


std::vector<double> generate_point_sigmas_by_raw_parameters(CommonStrokingParams params, size_t w, size_t h);