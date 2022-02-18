#pragma once


#include <utility>

#include "painter_pch.h"
#include "data/stroke.h"
#include "data/performance_statistics.h"

/**
 * `Workers` are inherited from `BaseWorker`.
 * They optimize strokes for a given image
 *
 * Workers are typically used in higher order abstractions
 * (e.g. zone splitters and launchers)
 */
class BaseWorker {
 public:
  BaseWorker() = default;

  explicit BaseWorker(fs::path logging_path, bool verbose) : logging_path(std::move(logging_path)),
                                                             verbose(verbose) {}

  // Setup
  virtual void set_basic_strokes(const std::vector<Stroke>& strokes) = 0;

  // Run
  virtual void run_one_iteration() = 0;

  virtual void run_remaining_iterations() = 0;

  // Get output
  virtual const std::vector<double>& get_best_genome() = 0;

  // Diagnostic
  [[nodiscard]] virtual PerformanceStatistics get_statistics() const = 0;

  virtual void save_fitness_dynamic() = 0;

 protected:
  // Logging
  fs::path logging_path;
  bool verbose = true;
};