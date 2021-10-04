#pragma once

#include <Annealing/annealing_optimize.h>

#include "painter_pch.h"
#include "data/color.h"
#include "optimization_adapters/annealing/all.h"
#include "launch/workers/simple.h"


class AnnealingWorker : public SimpleWorker {
 public:
  using ParametersType = AnnealingStrokingParams;

  AnnealingWorker(const Image& image, const CommonStrokingParams& stroking_params,
                  const AnnealingStrokingParams& annealing_params,
                  const fs::path& logging_path = fs::path{painter_base_path} / "log" / "latest",
                  bool verbose = true);

  void set_basic_strokes(const std::vector<Stroke>& strokes) override;

  /// Run methods
  void run_one_iteration() override;

  void run_remaining_iterations() override;

  const std::vector<double>& get_best_genome() override;


  /// Diagnostics
  AlgorithmStatistics get_efficiency_account() const override { return error_function.get_efficiency_account(); }

  void show_fitness_dynamic() override;

 private:
  // Image
  Image initial_image;
  size_t image_w;
  size_t image_h;

  // Params
  CommonStrokingParams common_stroking_params;
  AnnealingStrokingParams annealing_stroking_params;

  // Operations
  FitnessFunction error_function;
  AnnealingGenomeMutator mutator;
  AnnealingGenomeGenerator generator;
  AnnealingLoggingCallback logger;

  // Results
  AnnealingOptimizationOutput<double> result;

  // Logging
  bool verbose;
  fs::path logging_path;
};

