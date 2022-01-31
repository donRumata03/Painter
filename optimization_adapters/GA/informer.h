#pragma once

#include "painter_pch.h"
#include "utils/image.h"
#include "rasterization/stroke.h"

#include "optimization_adapters/utils.h"
#include "utils/logger.h"


class GaInformer {
  const Image& initial_image;
  size_t total_iterations = 0;
  bool enable_console_output = true;

 public:
  explicit GaInformer(const Image& image, size_t _total_iterations, bool enable_console_output = true)
          : initial_image(image), total_iterations(_total_iterations), enable_console_output(enable_console_output) {}

  void operator()(size_t GA_iterations, double best_fitness_value, const std::vector<double>& best_stroke_genome) {
    //if (enable_console_output)
    //LogInfo("GA") << "GA percent: " << percent_plotter(GA_iterations, total_iterations, 1) << "; Best MSE of error function is: " << 1 / best_fitness_value;

    // std::cout << best_stroke_genome << std::endl;

    return;

    // Show image:
    auto strokes = Packer::unpack_stroke_data_buffer(best_stroke_genome);
    colorize_strokes(strokes, initial_image);

    Image new_image = make_default_image(initial_image.cols, initial_image.rows);
    rasterize_strokes(new_image, strokes);

    show_image_in_system_viewer(new_image);

  }
};

