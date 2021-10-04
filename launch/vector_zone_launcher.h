#pragma once


#include "painter_pch.h"

#include "operations/error.h"
#include "operations/image/segregation.h"
#include "operations/image/adaptive_params.h"
#include "operations/color/optimization.h"
#include "launch/workers/ga.h"
#include "launch/workers/annealing.h"
#include "vector_graphics/svg_service.h"
#include "utils/logger.h"
#include "utils/format.h"


/**
 * Is responsible for dividing the image into several zones
 * following the partitioning made by a pasteurizer such as Adobe Illustrator
 * and expressed in the SVG image given.
 *
 *
 * The ability of running optimizers for multiple zones in parallel is supported.
 *
 * For example, we have 10 worker threads for computing error function
 * and ≈ 10'000 optimization tasks (1 for each zone: to solve the Worker's task for the specified region)
 *
 * It means that it's more than reasonable to distribute the tasks between threads.
 */

template <class OptimizerType>
class VectorZoneLauncher {
 public:
  using OptimizerParameters = typename OptimizerType::ParametersType;


  VectorZoneLauncher(const fs::path& image_path, const CommonStrokingParams& stroking_params,
                 const OptimizerParameters& custom_parameters,
                 const Canvas& canvas, bool parallelize = false,
                 size_t worker_thread_number = std::thread::hardware_concurrency() - 2,
                 fs::path logging_path = fs::path{painter_base_path} / "log" / "latest");

  void run();

  [[nodiscard]] AlgorithmStatistics get_efficiency_account() const { return efficiency_account; }

  std::vector<ColoredStroke> get_final_strokes(Units units = Units::PX, bool shift_strokes = false);

  cv::Size get_image_size() const { return svg_service->get_workspace_size(); }

 private:
  void worker_function(size_t thread_index);

  Image initial_image;
  std::optional<Canvas> canvas;
  fs::path logging_path;

  CommonStrokingParams stroking_params;
  OptimizerParameters optimizer_parameters;

  size_t zone_number = 0;
  std::vector<size_t> stroke_distribution;

  // Threading
  bool is_threaded = false;
  size_t thread_worker_number;
  static_thread_pool thread_pool;

  std::mutex common_worker_data_mutex;
  std::vector<std::pair<size_t, size_t>> thread_zone_distribution;

  // The common data being modified
  std::optional<SvgService> svg_service;
  std::optional<ZoneResourceDistributor> res_distributor;

  std::vector<ColoredStroke> collected_strokes;

  // For statistics
  AlgorithmStatistics efficiency_account;
};


template <class OptimizerType>
VectorZoneLauncher<OptimizerType>::VectorZoneLauncher(const fs::path& image_path,
                                                      const CommonStrokingParams& stroking_params,
                                                      const OptimizerParameters& custom_parameters,
                                                      const Canvas& canvas,
                                                      bool parallelize,
                                                      size_t worker_thread_number, fs::path logging_path)
        : logging_path(std::move(logging_path)), stroking_params(stroking_params),
          optimizer_parameters(custom_parameters),
          canvas(canvas), is_threaded(parallelize), thread_worker_number(worker_thread_number) {
  // Clear the old log
  ensure_log_cleared(this->logging_path);

  // Determine the number of zones and what the zones actually are
  svg_service.emplace(image_path,
                      canvas,
                      (this->stroking_params.units == Units::MM ? stroking_params.stroke_width.second :
                       canvas.px2mm(stroking_params.stroke_width.second)) / 2);
  svg_service->split_paths();

  initial_image = svg_service->get_raster_original_image();
  save_image(initial_image, (fs::path(painter_base_path) / "log" / "latest" / "original.png").string());

  zone_number = svg_service->get_regions_count();

  if (!this->stroking_params.use_absolute_values) {
    this->stroking_params = switch_to_absolute_values(this->stroking_params, initial_image.cols, initial_image.rows);
  } else if (this->stroking_params.use_absolute_values && this->stroking_params.units == Units::MM) {
    this->stroking_params.stroke_length = canvas.mm2px(this->stroking_params.stroke_length);
    this->stroking_params.stroke_width = canvas.mm2px(this->stroking_params.stroke_width);
    LogInfo("Vector Zone Launcher") << "Limits for stroke length: " << this->stroking_params.stroke_length
                                    << " px, for stroke width: " << this->stroking_params.stroke_width << " px";
  }

  res_distributor.emplace(svg_service.value(), stroking_params.canvas_color);
  stroke_distribution = res_distributor->distribute_resource(stroking_params.stroke_number, 1);

  // Make task distribution if parallel

  if (parallelize) {
    thread_zone_distribution = distribute_task_ranges(zone_number, worker_thread_number);
    thread_pool.init(worker_thread_number,
                     [&](size_t thread_index) { this->worker_function(thread_index); });
  } else {
    thread_zone_distribution = {
            {0ULL, zone_number}
    };
  }
}


template <class OptimizerType>
void VectorZoneLauncher<OptimizerType>::worker_function(size_t thread_index) {
  auto job_range = thread_zone_distribution[thread_index];

  LogConsoleInfo("Vector Zone Launcher", get_current_thread_info(thread_index))
          << "Started, job range: " << get_range_string(job_range);

  for (size_t job_index = job_range.first; job_index < job_range.second; ++job_index) {

    std::optional<OptimizerType> optimizer;
    VectorRegion region;

    {
      // Acquire a mutex to work with data, which is common for all the worker threads
      // this particular data will be used to initialize and launch data
      std::lock_guard<std::mutex> locker(common_worker_data_mutex);

      region = svg_service->get_region(job_index);

      Image image = region.image;
      auto params = this->stroking_params;

      params.stroke_number = stroke_distribution[job_index];
      params.use_constant_color = true;
      params.stroke_color = region.color;

      LogInfo("Vector Zone Launcher", get_current_thread_info(thread_index))
              << "Stroking region #" << job_index
              << " (" << params.stroke_number << " strokes, " << params << " color)";

      optimizer.emplace(image, params, this->optimizer_parameters,
                        this->logging_path / "stroking" / ("region" + std::to_string(job_index)), false);
    }

    optimizer->run_remaining_iterations();

    {
      // Acquire mutex to save the data collected:
      std::lock_guard<std::mutex> locker(common_worker_data_mutex);

      efficiency_account = efficiency_account + optimizer->get_efficiency_account();

      auto strokes = unpack_stroke_data_buffer(optimizer->get_best_genome());
      colorize_strokes(strokes, region.color);
      svg_service->shift_strokes_to_box(strokes, job_index);

      std::copy(strokes.begin(), strokes.end(), std::back_inserter(collected_strokes));

      LogInfo("Vector Zone Launcher", get_current_thread_info(thread_index)) << "Finish stroking region #" << job_index;
      Logger::UpdateProgress();
    }
  }

  LogConsoleSuccess("Vector Zone Launcher", get_current_thread_info(thread_index))
          << "Ended, job range: " << get_range_string(job_range);
}


template <class OptimizerType>
void VectorZoneLauncher<OptimizerType>::run() {
  LogConsoleInfo("Vector Zone Launcher") << "Run worker(s)";
  Logger::NewProgress(zone_number);
  if (is_threaded) {
    cv::setNumThreads(0);

    thread_pool.compute();

    cv::setNumThreads(-1); // Reset to system defaults
  } else {
    // Just do all the work with this particular thread:
    worker_function(0);
  }
  LogConsoleSuccess("Vector Zone Launcher") << "End running";
}


template <class OptimizerType>
std::vector<ColoredStroke> VectorZoneLauncher<OptimizerType>::get_final_strokes(Units units, bool shift_strokes) {
  std::vector<ColoredStroke> strokes(collected_strokes.size());
  std::copy(collected_strokes.begin(), collected_strokes.end(), strokes.begin());
  svg_service->transform_strokes_into_mm(strokes);

  if (shift_strokes) svg_service->shift_strokes_to_canvas(strokes);
  if (units == Units::PX) svg_service->transform_strokes_into_px(strokes);

  return strokes;
}