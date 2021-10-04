#pragma once


#include <painter_pch.h>

#include "data/common_stroking_params.h"
#include "operations/error.h"
#include "operations/color/optimization.h"
#include "operations/image/segregation.h"
#include "optimization_adapters/GA/all.h"
#include "launch/workers/simple.h"


/**
 * Class that wraps dividing the image into a couple of zones and launching
 */
template <class Worker>
class MultizoneLauncher {

 public:
  explicit MultizoneLauncher(Image _image, size_t _zones_x, size_t _zones_y, double overlay_percent,
                             const CommonStrokingParams& stroking_params, typename Worker::ParametersType params);

  bool process_one_cell();

  void run();

  Image get_resultant_image();

  void save_result(const fs::path& filename);


  // Memory management stuff
  MultizoneLauncher(const MultizoneLauncher&) = delete;

  MultizoneLauncher(MultizoneLauncher&&) = delete;

  MultizoneLauncher operator=(const MultizoneLauncher&) = delete;

  MultizoneLauncher operator=(MultizoneLauncher&&) = delete;

  ~MultizoneLauncher() = default;


 private:
  std::vector<double> glue_best_genomes();

 private:
  ImageZones zones;
  std::vector<std::vector<std::shared_ptr<Worker>>> workers;
  std::vector<std::vector<bool>> workers_ready;

  Image image;

  size_t epochs_performed = 0;

  /// Run-time constants:
  size_t image_w = size_t(-1);
  size_t image_h = size_t(-1);

  size_t zones_x = size_t(-1);
  size_t zones_y = size_t(-1);
};


template <class Worker>
MultizoneLauncher<Worker>::MultizoneLauncher(Image _image, size_t _zones_x, size_t _zones_y, double overlay_percent,
                                             const CommonStrokingParams& stroking_params,
                                             typename Worker::ParametersType custom_params)
        : zones(split_image_into_zones(_image, schedule_image_splitting(_image.cols, _image.rows, _zones_x, _zones_y,
                                                                        overlay_percent))),
          zones_x(_zones_x), zones_y(_zones_y) {
  image_w = this->image.cols;
  image_h = this->image.rows;

  image = std::move(_image);

  // Init workers
  std::vector<bool> worker_ready_mask(zones_y, false);
  workers_ready.assign(zones_x, worker_ready_mask);

  workers.reserve(zones_x);

  for (size_t worker_x_index = 0; worker_x_index < zones_x; ++worker_x_index) {
    auto& worker_col = workers.emplace_back();
    worker_col.reserve(zones_y);

    for (size_t worker_y_index = 0; worker_y_index < zones_y; ++worker_y_index) {
      std::string this_path =
              "x_index=" + std::to_string(worker_x_index) + ",y_index=" + std::to_string(worker_y_index);

      std::shared_ptr<Worker> this_worker = std::make_shared<Worker>(
              zones.images[worker_x_index][worker_y_index], stroking_params, custom_params,
              painter_base_path / "log" / "latest" / this_path
      );

      worker_col.emplace_back(std::move(this_worker));
    }
  }


}

template <class Worker>
bool MultizoneLauncher<Worker>::process_one_cell() {
  // Determine cell number
  bool found_incomplete_cell = false;
  size_t cell_x = 0, cell_y = 0;

  for (size_t x = 0; x < zones_x; ++x) {
    for (size_t y = 0; y < zones_y; ++y) {
      if (!workers_ready[x][y]) {
        found_incomplete_cell = true;
        cell_x = x;
        cell_y = y;
        break;
      }
    }
    if (found_incomplete_cell) break;
  }


  if (!found_incomplete_cell) {
    LogConsoleWarning("Multizone Launcher") << "Didn't find any non-processed cells => finishing...";
    return false;
  }

  LogConsoleInfo("Multizone Launcher") << "Processing new cell: x = " << cell_x << ", y = " << cell_y
                                       << ": " << zones.zone_descriptor.get_2d_cells()[cell_x][cell_y];

  auto& worker = workers[cell_x][cell_y];
  worker->run_remaining_iterations();

  worker->get_statistics().print_diagnostic_information();
  workers_ready[cell_x][cell_y] = true;

  return true;
}


template <class Worker>
void MultizoneLauncher<Worker>::run() {
  while (process_one_cell()) {}
}


template <class Worker>
std::vector<double> MultizoneLauncher<Worker>::glue_best_genomes() {
  std::vector<double> res;
  size_t total_buffer_size = zones_x * zones_y * workers[0][0]->get_best_genome().size();

  LogConsoleInfo("Multizone Launcher") << "Copying " << total_buffer_size << " doubles";

  res.reserve(total_buffer_size);

  size_t x_index = 0;
  for (auto& col : workers) {
    size_t y_index = 0;
    for (auto& worker : col) {
      const auto& this_cell_descriptor = zones.zone_descriptor.get_2d_cells()[x_index][y_index];

      size_t this_x_shift = this_cell_descriptor.min_x;
      size_t this_y_shift = this_cell_descriptor.min_y;

      auto strokes = unpack_stroke_data_buffer(worker->get_best_genome());

      std::for_each(strokes.begin(), strokes.end(),
                    [this_x_shift, this_y_shift](Stroke& stroke) {
                      shift_stroke(stroke, {double(this_x_shift), double(this_y_shift)});
                    });

      auto shifted_buffer = pack_stroke_data(strokes);

      std::copy(shifted_buffer.begin(), shifted_buffer.end(), std::back_inserter(res));

      y_index++;
    }
    x_index++;
  }

  assert(res.size() == total_buffer_size);

  return res;
}

template <class Worker>
Image MultizoneLauncher<Worker>::get_resultant_image() {
  return stroke_buffer_to_image(glue_best_genomes(), image);
}


template <class Worker>
void MultizoneLauncher<Worker>::save_result(const fs::path& filename) {
  save_image(get_resultant_image(), filename);
}

