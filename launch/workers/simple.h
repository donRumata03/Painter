#pragma once


#include "painter_pch.h"
#include "data/algorithm_statistics.h"


class SimpleWorker {
 public:

  // Run
  virtual void run_one_iteration() = 0;

  virtual void run_remaining_iterations() = 0;

  // Get output
  virtual const std::vector<double>& get_best_genome() = 0;

  // Diagnostic
  [[nodiscard]] virtual AlgorithmStatistics get_efficiency_account() const = 0;

  virtual void show_fitness_dynamic() = 0;

 protected:
  bool enable_console_output = true;
};