#include "fitness.h"


FitnessFunction::FitnessFunction(const ImageStrokingData& imageData, size_t strokes, bool is_run_sequentially,
                                 bool inverse, const Color& canvas_color)
        : imageData(imageData), total_stroke_number(strokes), is_run_sequentially(is_run_sequentially),
          canvas_color(canvas_color), inverse(inverse) {
  w = imageData.image.cols;
  h = imageData.image.rows;

  if (is_run_sequentially) {
    personal_buffer = make_default_image(w, h, canvas_color);
  }
}

double FitnessFunction::operator()(const std::vector<double>& stroke_data_buffer) const {
  assert(total_stroke_number * 7 == stroke_data_buffer.size());

  Timer computation_timer;

  auto strokes = Packer::unpack_stroke_data_buffer(stroke_data_buffer);
  colorize_strokes(strokes, imageData);

  // Compute MSE; OpenCV reference counting system manages the memory properly
  Image this_buffer = personal_buffer;

  if (!is_run_sequentially) { // Parallel
    this_buffer = buffer_holder->get_buffer_for_thread(w, h, canvas_color);
  }

  rasterize_strokes(this_buffer, strokes);
  auto painting = RasterizedPainting({static_cast<int>(w), static_cast<int>(h)}, canvas_color);
  rasterize_strokes(painting, strokes);
  Image imposition = painting.get_imposition_matrix();

  double MSE = image_mse(imageData.image, this_buffer);
  double MSE_imposition = image_mse(imposition, make_default_image(w, h, 0));

  MSE += MSE_imposition;

  fill_image(this_buffer, canvas_color, false); // Clear

  rt_counter.total_time_seconds->operator+=(computation_timer.get_time(Timer::time_units::seconds));
  rt_counter.total_runs->operator++();

  return !inverse ? MSE : (1 / MSE);
}

PerformanceStatistics FitnessFunction::get_efficiency_account() const {
  return PerformanceStatistics{
          .computations_performed = *rt_counter.total_runs,
          .pixels_processed = *rt_counter.total_runs * w * h,
          .time_spent_computing = (*rt_counter.total_time_seconds),
  };
}