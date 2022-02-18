#pragma once


#include "painter_pch.h"

#include "data/stroke_limits.h"
#include "data/common_stroking_params.h"
#include "operations/stroke/limits.h"
#include "optimization_adapters/GA/all.h"
#include "rasterization/stroke.h"
#include "launch/workers/simple.h"
#include "utils/image.h"


class GaWorker : public BaseWorker {
 public:
  using ParametersType = GaStrokingParams;

  GaWorker(const Image& image,
           const CommonStrokingParams& stroking_params, const GaStrokingParams& optimizer_params,
           const fs::path& logging_path = fs::path(painter_base_path) / "log" / "latest",
           bool enable_console_output = true);

  void set_basic_strokes(const std::vector<Stroke>& strokes) override;

  void run_one_iteration() override;

  void run_remaining_iterations() override;

  void save_fitness_dynamic() override;

  PerformanceStatistics get_statistics() const override { return fitness_function.get_efficiency_account(); }

  const std::vector<double>& get_best_genome() override;

 private:
  Image initial_image;
  size_t image_w;
  size_t image_h;

  // Params
  CommonStrokingParams stroking_params;
  GaStrokingParams GA_params;

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
