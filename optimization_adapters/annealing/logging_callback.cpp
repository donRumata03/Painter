#include "logging_callback.h"


void AnnealingLoggingCallback::operator()(const std::vector<double>& stroke_set, size_t this_iteration, double error) {
  fs::path filename = updating_path / (std::to_string(this_iteration) + ".png");
  LogInfo("Annealing", "Log") << "Update logged:  " << filename.string();
  if (verbose) Logger::UpdateProgress(this_iteration - prev_iteration);
  save_stroke_buffer_as_image(stroke_set, imageData, filename, canvas_color);

  prev_iteration = this_iteration;
}
