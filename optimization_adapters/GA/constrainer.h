#pragma once

#include "painter_pch.h"
#include "data/stroke_limits.h"
#include "operations/stroke/limits.h"
#include "optimization_adapters/utils.h"


struct GaConstrainer {
  StrokeLimits limits{};

  GaConstrainer() = default;

  explicit GaConstrainer(const StrokeLimits& descriptor) : limits(descriptor) {}

  void operator()(GA::Population& population, const std::vector<std::pair<double, double>>& constraints) const {
    // Ignore given constraints
    for (auto& genome : population) {
      auto strokes = unpack_stroke_data_buffer(genome);

      for (auto& stroke : strokes) {
        limits.constrain_stroke_to_requirements(stroke);
      }
      // std::vector<double> stroke_data = pack_stroke_data(strokes);
      genome = pack_stroke_data(strokes); // std::move(stroke_data);
    }
  }
};

