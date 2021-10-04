#pragma once

#include "painter_pch.h"

#include "optimization_adapters/utils.h"
#include "rasterization/stroke.h"
#include "utils/logger.h"


/**
 * Renders and logs all the intermediate states as images to folders to a given one.
 * Does this and copies all the generated folder to a new one, specially for generated  in destructor
 */
class GaLoggingCallback {
  ImageStrokingData imageData;
  Color canvas_color{};

  fs::path path_for_logging;
  fs::path path_for_best_genomes;

  bool verbose = true;
  bool detalized_logging = false;
  double logged_genome_rate = 1.;


 public:
  GaLoggingCallback() = default;

  GaLoggingCallback(ImageStrokingData imageData, size_t total_epoch, const std::string& path_for_logging,
                    double logged_percent, bool verbose = true, bool log_precisely = true, Color canvas_color = {});

  void operator()(const GA::Population& population, size_t epoch_index, logging_type this_logging_type);

  ~GaLoggingCallback() = default;
};


