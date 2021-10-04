#pragma once


#include <utility>

#include "painter_pch.h"
#include "data/stroke.h"
#include "data/algorithm_statistics.h"


class SimpleWorker {
 public:
  SimpleWorker() = default;

  explicit SimpleWorker(fs::path logging_path, bool verbose) : logging_path(std::move(logging_path)),
                                                               verbose(verbose) {}

  // Setup
  virtual void set_basic_strokes(const std::vector<Stroke>& strokes) = 0;

  // Run
  virtual void run_one_iteration() = 0;

  virtual void run_remaining_iterations() = 0;

  // Get output
  virtual const std::vector<double>& get_best_genome() = 0;

  // Diagnostic
  [[nodiscard]] virtual AlgorithmStatistics get_efficiency_account() const = 0;

  virtual void show_fitness_dynamic() = 0;

 protected:
  // Logging
  fs::path logging_path;
  bool verbose = true;
};