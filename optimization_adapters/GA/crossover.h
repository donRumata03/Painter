#pragma once

#include "painter_pch.h"


class GaCrossover {

 public:
  GaCrossover() = default;

  std::vector<double>
  operator()(const std::pair<std::vector<double> *, std::vector<double> *>& parents, const normalizer& normalizer,
             const GA::crossover_mode mode) {
    // TODO: exchange some close strokes (select some region to mix strokes in it 50/50; all the other strokes will be from one of the stroke-sets)
    // This is done to make it clear for optimizer if the mizture is good or not (for full 50/50 swap it won't be clear because there will be both a couple of bad and good region-mixtures)
    // But this is probably an overkill for small zones. May be, for them we'll just miz full 50/50

    return (pythonic_random() >= 0.5) ? (*parents.first) : (*parents.second);
  }
};
