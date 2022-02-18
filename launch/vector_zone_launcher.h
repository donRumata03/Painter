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

class VectorZoneLauncher {
 public:


  VectorZoneLauncher(const fs::path& image_path, const CommonStrokingParams& stroking_params,
                     bool parallelize = false, size_t worker_thread_number = std::thread::hardware_concurrency() - 2,
                     fs::path logging_path = fs::path{painter_base_path} / "log" / "latest");

  void run();

  [[nodiscard]] PerformanceStatistics get_efficiency_account() const { return efficiency_account; }

  std::vector<ColoredStroke> get_final_strokes(Units units = Units::PX, bool shift_strokes = false);

  cv::Size get_image_size() const { return svg_service->get_workspace_size(); }

 private:
  void worker_function(size_t thread_index);

  Image initial_image;
  Canvas canvas;
  fs::path logging_path;

  CommonStrokingParams stroking_params;

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
  PerformanceStatistics efficiency_account;
};


inline VectorZoneLauncher::VectorZoneLauncher(const fs::path& image_path,
                                       const CommonStrokingParams& stroking_params,
                                       bool parallelize,
                                       size_t worker_thread_number, fs::path logging_path)
        : logging_path(std::move(logging_path)), stroking_params(stroking_params),
          canvas(stroking_params.canvas), is_threaded(parallelize), thread_worker_number(worker_thread_number) {

  if (this->stroking_params.is_relative) {
    this->stroking_params = switch_to_absolute_values(this->stroking_params, initial_image.cols, initial_image.rows);
  } else if (this->stroking_params.units == Units::MM) { // TODO: make function for switch mm/px in stroking params
    this->stroking_params.stroke_length.first = canvas.mm2px(this->stroking_params.stroke_length.first);
    this->stroking_params.stroke_length.second = canvas.mm2px(this->stroking_params.stroke_length.first);
    this->stroking_params.stroke_width.first = canvas.mm2px(this->stroking_params.stroke_width.first);
    this->stroking_params.stroke_width.second = canvas.mm2px(this->stroking_params.stroke_width.second);

    LogInfo("Vector Zone Launcher") << "Limits for stroke length: " << this->stroking_params.stroke_length
                                    << " px, for stroke width: " << this->stroking_params.stroke_width << " px";
  }

  // Determine the number of zones and what the zones actually are
  svg_service.emplace(image_path,
                      canvas,
                      (this->stroking_params.units == Units::MM ? stroking_params.stroke_width.second :
                       canvas.px2mm(stroking_params.stroke_width.second)) / 2);
  svg_service->split_paths();

  initial_image = svg_service->get_raster_original_image();
  save_image(initial_image, (fs::path(painter_base_path) / "log" / "latest" / "original.png").string());

  zone_number = svg_service->get_regions_count();


  res_distributor.emplace(svg_service.value(), stroking_params.canvas_color);
  stroke_distribution = res_distributor->distribute_resource(stroking_params.stroke_number, 1);

  // TODO: Make task distribution if parallel

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


inline void VectorZoneLauncher::worker_function(size_t thread_index) {
  auto job_range = thread_zone_distribution[thread_index];

  LogConsoleInfo("Vector Zone Launcher", get_current_thread_info(thread_index))
          << "Started, job range: " << get_range_string(job_range);

  for (size_t job_index = job_range.first; job_index < job_range.second; ++job_index) {
    size_t pipeline_index = 0;
    std::vector<ColoredStroke> strokes;
    VectorRegion region;
    CommonStrokingParams params;
    Image image;

    {
      // Acquire a mutex to work with data, which is common for all the worker threads
      // this particular data will be used to initialize and launch data
      std::lock_guard<std::mutex> locker(common_worker_data_mutex);

      region = svg_service->get_region(job_index);

      image = region.image;
      params = this->stroking_params;

      params.stroke_number = stroke_distribution[job_index];
      params.use_constant_color = true;
      params.stroke_color = region.color;
    }

    LogInfo("Vector Zone Launcher", get_current_thread_info(thread_index))
            << "Stroking region #" << job_index
            << " (" << params.stroke_number << " strokes, " << params.stroke_color << " color)";

    for (auto& chain : stroking_params.sequence) {
      std::unique_ptr<BaseWorker> optimizer;

      auto cur_logging_path = logging_path / "stroking" / ("region" + std::to_string(job_index)) /
                              ("chain" + std::to_string(pipeline_index));

      if (chain.index() == 0) { // GA
        optimizer = std::make_unique<GaWorker>(image, params, std::get<GaStrokingParams>(chain), cur_logging_path,
                                               false);
      } else if (chain.index() == 1) { // Annealing
        optimizer = std::make_unique<AnnealingWorker>(image, params, std::get<AnnealingStrokingParams>(chain),
                                                      cur_logging_path, false);
      }
      LogInfo("Vector Zone Launcher", get_current_thread_info(thread_index), "Region #" + std::to_string(job_index))
              << "Start " << (chain.index() == 0 ? "GA" : "annealing") << " (chain " << (pipeline_index + 1) << "/"
              << params.sequence.size() << ")";

      if (!strokes.empty()) {
        optimizer->set_basic_strokes(get_strokes_base(strokes));
      }

      optimizer->run_remaining_iterations();

      strokes = Packer::unpack_stroke_data_buffer(optimizer->get_best_genome());

      {
        std::lock_guard<std::mutex> locker(common_worker_data_mutex);

        efficiency_account = efficiency_account + optimizer->get_statistics();
        optimizer->save_fitness_dynamic();
      }
      LogInfo("Vector Zone Launcher", get_current_thread_info(thread_index), "Region #" + std::to_string(job_index))
              << "Chain #" << (pipeline_index + 1) << " finished";
      pipeline_index++;
    }

    colorize_strokes(strokes, region.color);
    svg_service->shift_strokes_to_box(strokes, job_index);

    {
      // Acquire mutex to save the data collected
      std::lock_guard<std::mutex> locker(common_worker_data_mutex);

      std::copy(strokes.begin(), strokes.end(), std::back_inserter(collected_strokes));
    }

    LogInfo("Vector Zone Launcher", get_current_thread_info(thread_index)) << "Finish stroking region #"
                                                                           << job_index;
    Logger::UpdateProgress();
  }

  LogConsoleSuccess("Vector Zone Launcher", get_current_thread_info(thread_index))
          << "Ended, job range: " << get_range_string(job_range);
}


inline void VectorZoneLauncher::run() {
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
  efficiency_account.print_diagnostic_information();
}


inline std::vector<ColoredStroke> VectorZoneLauncher::get_final_strokes(Units units, bool shift_strokes) {
  std::vector<ColoredStroke> strokes(collected_strokes.size());
  std::copy(collected_strokes.begin(), collected_strokes.end(), strokes.begin());
  svg_service->transform_strokes_into_mm(strokes);

  if (shift_strokes) svg_service->shift_strokes_to_canvas(strokes);
  if (units == Units::PX) svg_service->transform_strokes_into_px(strokes);

  return strokes;
}
