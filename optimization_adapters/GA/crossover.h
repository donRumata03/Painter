#pragma once

#include "painter_pch.h"


class GaCrossover {

 public:
  GaCrossover() = default;

  std::vector<double>
  operator()(const std::pair<std::vector<double> *, std::vector<double> *>& parents, const normalizer& normalizer,
             const GA::crossover_mode mode) {
    // todo: exchange some close stroke's points

    return (pythonic_random() >= 0.5) ? (*parents.first) : (*parents.second);
  }
};
