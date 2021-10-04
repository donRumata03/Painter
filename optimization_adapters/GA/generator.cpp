#include "generator.h"


std::vector<std::pair<double, double>> generate_point_ranges_for_stroke_genome(
        size_t stroke_number, const point& image_size, const std::pair<double, double>& width_range) {
  assert(sizeof(Stroke) == 7 * sizeof(double));
  std::vector<std::pair<double, double>> res(stroke_number * 7);

  std::pair<double, double> x_range = {0., double(image_size.x)};
  std::pair<double, double> y_range = {0., double(image_size.y)};

  for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
    size_t base_buffer_index = stroke_index * 7;

    res[base_buffer_index + 0] = res[base_buffer_index + 2] = res[base_buffer_index + 4] = x_range;
    res[base_buffer_index + 1] = res[base_buffer_index + 3] = res[base_buffer_index + 5] = y_range;

    res[base_buffer_index + 6] = width_range;
  }

  return res;
}

std::vector<double> generate_point_sigmas_for_stroke_genome(size_t stroke_number, const point& image_size,
                                                            double absolute_sigma_for_coords,
                                                            double absolute_sigma_for_width) {
  assert(sizeof(Stroke) == 7 * sizeof(double));
  std::vector<double> res(stroke_number * 7);

  double x_sigma = absolute_sigma_for_coords;
  double y_sigma = absolute_sigma_for_coords;

  for (size_t stroke_index = 0; stroke_index < stroke_number; ++stroke_index) {
    size_t base_buffer_index = stroke_index * 7;

    // Coords:
    res[base_buffer_index + 0] = res[base_buffer_index + 2] = res[base_buffer_index + 4] = x_sigma;
    res[base_buffer_index + 1] = res[base_buffer_index + 3] = res[base_buffer_index + 5] = y_sigma;

    res[base_buffer_index + 6] = absolute_sigma_for_width;
  }

  return res;
}

CommonStrokingParams switch_to_absolute_values(const CommonStrokingParams& params, size_t w, size_t h) {
  if (!params.is_relative) return params;

  CommonStrokingParams new_params = params;

  new_params.stroke_width.first *= (double) (w + h) / 2.;
  new_params.stroke_width.second *= (double) (w + h) / 2.;

  new_params.stroke_length.first *= (double) (w + h) / 2.;
  new_params.stroke_length.second *= (double) (w + h) / 2.;

  new_params.is_relative = false;

  return new_params;
}

StrokeLimits generate_stroke_limits_by_raw_parameters(CommonStrokingParams params, size_t w, size_t h) {
  auto absolute_params = switch_to_absolute_values(params, w, h);

  auto limits = StrokeLimits{ // TODO: limit box width/height != limits for stroke_length
          .min_dx     = std::max(absolute_params.stroke_length.first, 0.),
          .max_dx     = std::min(absolute_params.stroke_length.second, (double) w),

          .min_dy     = std::max(absolute_params.stroke_length.first, 0.),
          .max_dy     = std::min(absolute_params.stroke_length.second, (double) h),

          .min_width  = absolute_params.stroke_width.first,
          .max_width  = absolute_params.stroke_width.second,

          .image_rectangle = RangeRectangle<double>{0., double(w), 0., double(h)}
  };

  return limits;
}


std::vector<std::pair<double, double>>
generate_point_ranges_by_raw_parameters(CommonStrokingParams params, size_t w, size_t h) {
  auto absolute_params = switch_to_absolute_values(params, w, h);

  return generate_point_ranges_for_stroke_genome(
          absolute_params.stroke_number,
          {double(w), double(h)},
          absolute_params.stroke_width
  );
}


std::vector<double> generate_point_sigmas_by_raw_parameters(CommonStrokingParams params, size_t w, size_t h) {
  auto absolute_params = switch_to_absolute_values(params, w, h);

  return generate_point_sigmas_for_stroke_genome(
          params.stroke_number,
          {double(w), double(h)},
          absolute_params.sigma * (absolute_params.stroke_length.first + absolute_params.stroke_length.first) / 2.,
          absolute_params.sigma * (absolute_params.stroke_width.first + absolute_params.stroke_width.first) / 2.
  );
}