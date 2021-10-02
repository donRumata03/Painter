#pragma once

#include "operations/stroke/limits.h"
#include "operations/stroke/generation.h"
#include "optimization_adapters/utils.h"
#include "optimization_adapters/GA/generator.h"


class GaPopulationGenerator {
  StrokeLimits limits;
  size_t strokes_in_image;

  GaGenomeGenerator m_generator;

 public:
  GaPopulationGenerator() = default;

  explicit GaPopulationGenerator(const StrokeLimits& limits, size_t stroke_number)
          : limits(limits), strokes_in_image(stroke_number), m_generator(limits, stroke_number) {}

  std::vector<std::vector<double>>
  operator()(const std::vector<std::pair<double, double>>& ranges, size_t amount) const {
    std::vector<std::vector<double>> res(amount);

    for (size_t image_index = 0; image_index < amount; ++image_index) {
      res[image_index] = m_generator(strokes_in_image * sizeof(Stroke) / sizeof(double));  // std::move(packed_strokes);
    }

    return res;
  }

};