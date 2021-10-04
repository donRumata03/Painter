#pragma once

#include "data/stroke_limits.h"
#include "optimization_adapters/utils.h"


/**
 * Callable object for mutation in GA
 */
class GaMutator {
  StrokeLimits limits{};
  double move_probability = -1;

 public:
  GaMutator() = default;

  explicit GaMutator(const StrokeLimits& limits, double move_probability) : limits(limits),
                                                                            move_probability(move_probability) {
    assert(move_probability >= 0 and move_probability <= 1);
  }

  void operator()(std::vector<double>& stroke_data_buffer, const std::vector<double>& sigmas, double target_gene_number,
                  const normalizer& norma) const {
    auto strokes = unpack_stroke_data_buffer(stroke_data_buffer);

    if (pythonic_random() < move_probability) {
      assert(sizeof(Stroke) / sizeof(double) == 7);
      assert(stroke_data_buffer.size() == sigmas.size());

      double per_gene_move_probability = target_gene_number / double(stroke_data_buffer.size());

      for (auto& this_stroke : strokes) {
        bool move_or_not_to_move = pythonic_random() < per_gene_move_probability;
        if (!move_or_not_to_move) {
          continue;
        }

        // Move:
        double x_sigma = sigmas[0];
        double y_sigma = sigmas[1];

        double dx = norma.generate(0, x_sigma);
        double dy = norma.generate(0, y_sigma);

        shift_stroke(this_stroke, {dx, dy});
      }
    } else {
      // Randomly mutate
      return GA::mutate(stroke_data_buffer, sigmas, target_gene_number, norma);
    }
  }
};
