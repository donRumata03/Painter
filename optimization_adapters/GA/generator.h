#pragma once

#include "data/stroke_limits.h"
#include "operations/stroke/limits.h"
#include "operations/stroke/generation.h"
#include "optimization_adapters/utils.h"


class GaGenomeGenerator
{
  StrokeLimits limits;
  size_t strokes_in_image;

 public:
  GaGenomeGenerator() = default;
  explicit GaGenomeGenerator(const StrokeLimits& limits, size_t stroke_number)
          : limits(limits), strokes_in_image(stroke_number) {}

  std::vector<double> operator() (size_t amount) const
  {
    // Amount isn't the the number of strokes, it's the amount of pieces
    assert(not(amount % (sizeof(Stroke) / sizeof(double))));

    size_t number_of_strokes = amount / 7;
    assert(number_of_strokes == strokes_in_image);

    std::vector<Stroke> strokes(strokes_in_image);
    std::generate(strokes.begin(), strokes.end(), [this]() { return generate_stroke(limits); });


    return pack_stroke_data(strokes);
  }
};


