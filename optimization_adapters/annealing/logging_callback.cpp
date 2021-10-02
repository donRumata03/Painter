#include "logging_callback.h"


void AnnealingLoggingCallback::operator()(const std::vector<double>& stroke_set, size_t this_iteration, double error) const {
  // As for now, just have some probability of showing:
  if (not(pythonic_random() < logging_fraction)) {
    return;
  }

  fs::path filename = updating_path / (std::to_string(this_iteration) + ".png");
  if (enable_console_output) LogInfo("Annealing", "Log") << "Update logged:  " << filename.string();
  save_stroke_buffer_as_image(stroke_set, imageData, filename, canvas_color);
}
