#pragma once

#include "painter_pch.h"
#include "data/color.h"
#include "data/units.h"


struct OptimizerParams {
  auto operator<=>(const OptimizerParams& optimizer_params) const = default;
};

struct CommonStrokingParams {
  size_t stroke_number = 0;

  Units units = Units::MM;

  /// Limitations
  bool is_relative = false; // Is limit values are relative to canvas

  Limit stroke_length;
  Limit stroke_width;

  /// Probabilities
  double move_mutation_probability = 0.2;
  double sigma = 0.5;

  /// Colors
  Color canvas_color = {0., 0., 0.};
  Color stroke_color = {1., 1., 1.};
  bool use_constant_color = false; // Ignore optimization for strokes' color

  /// Optimizers sequence
  //std::vector<OptimizerParams> params;

  /// Utils
  double logging_percentage = 0.00;

  auto operator<=>(const CommonStrokingParams& common_stroking_params) const = default;
};


CommonStrokingParams load_params(const std::string& path);

void to_json(json& j, const CommonStrokingParams& params);
void from_json(const json& j, CommonStrokingParams& params);