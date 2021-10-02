#pragma once

#include <variant>

#include "painter_pch.h"
#include "data/color.h"
#include "data/units.h"


struct GaStrokingParams {
  size_t population_size = 0;
  size_t epoch_num = 0;

  bool allow_multithreading = false;

  [[nodiscard]] size_t computations_expected() const { return epoch_num * population_size; }
  auto operator<=>(const GaStrokingParams& params) const = default;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(GaStrokingParams, population_size, epoch_num, allow_multithreading)
};

struct AnnealingStrokingParams {
  size_t iterations = 0;
  double typical_temperature = 1.;
  double gene_mutation_fraction = 0.1;

  // No multithreading: Single annealing optimization operation can't be performed multithreadingly!

  [[nodiscard]] size_t computations_expected() const { return iterations; }
  auto operator<=>(const AnnealingStrokingParams& params) const = default;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnnealingStrokingParams, iterations, typical_temperature, gene_mutation_fraction)
};

using OptimizerParams = std::variant<GaStrokingParams, AnnealingStrokingParams>;


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
  std::vector<OptimizerParams> sequence;

  /// Utils
  double logging_percentage = 0.00;

  auto operator<=>(const CommonStrokingParams& common_stroking_params) const = default;
};


CommonStrokingParams load_params(const std::string& path);

void to_json(json& j, const CommonStrokingParams& params);
void from_json(const json& j, CommonStrokingParams& params);
