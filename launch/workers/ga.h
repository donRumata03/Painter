#pragma once


#include "painter_pch.h"

#include "data/stroke_limits.h"
#include "data/common_stroking_params.h"
#include "operations/stroke/limits.h"
#include "optimization_adapters/GA/all.h"
#include "rasterization/stroke.h"
#include "launch/workers/simple.h"
#include "utils/image.h"


struct GaStrokingParameters : public OptimizerParams {
  size_t population_size = 0;
  size_t epoch_num = 0;

  bool allow_multithreading = false;

  [[nodiscard]] size_t computations_expected() const { return epoch_num * population_size; }
};


class GaWorker : public SimpleWorker {
 public:
  using ParametersType = GaStrokingParameters;

  GaWorker(const Image& image,
            const CommonStrokingParams& stroking_params, const GaStrokingParameters& optimizer_params,
            const fs::path& logging_path = fs::path{painter_base_path} / "log" / "latest",
            bool enable_console_output = true);

  void run_one_iteration() override;

  void run_remaining_iterations() override;

  void show_fitness_dynamic() override;

  AlgorithmStatistics get_efficiency_account() const override { return fitness_function.get_efficiency_account(); }

  const std::vector<double>& get_best_genome() override;

 private:
  Image initial_image;
  size_t image_w;
  size_t image_h;

  // Params
  CommonStrokingParams stroking_params;
  GaStrokingParameters GA_params;

  StrokeLimits limits;

  // GA operations
  FitnessFunction fitness_function;
  GaConstrainer constrainer;
  GaPopulationGenerator generator;
  GaCrossover crossover;
  GaMutator mutator;

  GaLoggingCallback logger;

  // GA init data
  GA::GA_operation_set ga_operations;
  GA::continuous_GA_params ga_params;

  std::vector<std::pair<double, double>> point_ranges;
  std::vector<double> mutation_sigmas;

  // Optimizer itself
  std::optional<GA::GA_optimizer> optimizer;
};
