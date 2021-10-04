#pragma once

#include <utility>

#include "data/stroke.h"
#include "rasterization/stroke.h"
#include "utils/image.h"
#include "optimization_adapters/utils.h"


/**
 * Logs on some of the updates depending on the given hyperparameters
 *
 * Regulation ideas:
 * 	1. Log everything
 * 	2. Log some fraction of all precedents
 * 	3. Make an inverse exponential plan (iteration-logging_speed) using probable amount pow and log only if it's below plan
 *
 *
 * Usage example: Annealing
 */
class AnnealingLoggingCallback {
 public:
  AnnealingLoggingCallback() = default;

  AnnealingLoggingCallback(ImageStrokingData imageData, size_t total_iterations, double logging_percentage,
                              const fs::path& logging_path, Color canvas_color, bool verbose = true)
          : canvas_color(canvas_color), total_iterations(total_iterations), logging_fraction(logging_percentage),
            logging_path(logging_path), imageData(std::move(imageData)),
            verbose(verbose) {
    if (fs::exists(logging_path)) fs::remove_all(logging_path);

    fs::create_directories(logging_path);
    save_image(this->imageData.image, logging_path / "original.png");

    updating_path = logging_path / "updates";
    fs::create_directories(updating_path);

    LogInfo("Annealing", "Log") << "Initialized logger with path to updates: " << updating_path;
    if (verbose) Logger::NewProgress(total_iterations);
  }

  void operator()(const std::vector<double>& stroke_set, size_t this_iteration, double error);

 private:
  size_t total_iterations = 0;
  size_t prev_iteration = 0;
  double logging_fraction = 0.;

  ImageStrokingData imageData;

  Color canvas_color{};

  fs::path logging_path;
  fs::path updating_path;

  bool verbose;
};


