#pragma once

#include "data/stroke_limits.h"
#include "operations/stroke/limits.h"
#include "optimization_adapters/utils.h"
#include "optimization_adapters/GA/mutator.h"


/**
 * Callable object for mutation,
 * which automatically computes current sigmas and mutation gene number «on-fly» by «mutation amount»
 * (for example, it's used in annealing)
 */
class AnnealingGenomeMutator {
  StrokeLimits limits;

  /// Mutation intensiveness:
  std::vector<double> max_mutation_sigmas;
  double max_target_mutation_gene_amount = 0;
  double move_probability = -1;

  normalizer norma;
  GaMutator m_mutator{};

  bool constrain_after_mutation = false;

 public:
  AnnealingGenomeMutator() : norma(10'000) {}

  AnnealingGenomeMutator(StrokeLimits limits, std::vector<double> max_mutation_sigmas,
                         double max_target_mutation_gene_amount, double move_probability,
                         bool _constrain_after_mutation)
          : limits(limits), max_mutation_sigmas(std::move(max_mutation_sigmas)),
            max_target_mutation_gene_amount(max_target_mutation_gene_amount), move_probability(move_probability),
            norma(10'000), constrain_after_mutation(_constrain_after_mutation) {
    m_mutator = GaMutator(limits, move_probability);
  }

  std::vector<double> operator()(const std::vector<double>& stroke_data_buffer, double amount) const {
    double adjusted_amount = transfer_range(amount, {0., 1.}, {0.1, 1.});

    size_t genome_elements = stroke_data_buffer.size();
    assert(genome_elements == max_mutation_sigmas.size());
    size_t stroke_number = genome_elements / (sizeof(Stroke) / sizeof(double));

    std::vector<double> stroke_sigmas(max_mutation_sigmas.begin(), max_mutation_sigmas.end());
    // Decrease by multiplying by adjusted_amount
    std::transform(stroke_sigmas.begin(), stroke_sigmas.end(), stroke_sigmas.begin(),
                   [&adjusted_amount](double value) { return value * adjusted_amount; });

    double target_gene_number = max_target_mutation_gene_amount * adjusted_amount;

    // The actual mutation
    std::vector<double> result = stroke_data_buffer;
    m_mutator(result, stroke_sigmas, target_gene_number, norma);

    // Constrain if configured so
    if (constrain_after_mutation) {
      auto strokes = Packer::unpack_stroke_data_buffer(result);

      for (auto& stroke : strokes) {
        limits.constrain_stroke_to_requirements(stroke);
      }

      result = Packer::pack_stroke_data(strokes);
    }

    return result;
  }
};
