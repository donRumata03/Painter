#pragma once

#include "operations/error.h"
#include "operations/color/optimization.h"
#include "optimization_adapters/utils.h"
#include "data/algorithm_statistics.h"


struct RunTimeCounter {
  std::shared_ptr<std::atomic<size_t>> total_runs = std::make_shared<std::atomic<size_t>>(0);
  std::shared_ptr<std::atomic<double>> total_time_seconds = std::make_shared<std::atomic<double>>(0.);
};

class BufferBank {
 public:
  Image get_buffer_for_thread(size_t w, size_t h, const Color& canvas_color) {
    size_t thread_id = get_thread_id();

    // NOT Allocate the buffer if there is one
    {
      // Lock while copying all useful data
      std::lock_guard<std::mutex> lock(buffer_map_realloc_mutex);

      auto proposed_container = buffer_map.find(thread_id);
      if (proposed_container == buffer_map.end()) {
        // Allocate new buffer
        auto[iterator, some_bool] = buffer_map.insert({thread_id, make_default_image(w, h, canvas_color)});
        return iterator->second;
      } else {
        // Use existing buffer
        return proposed_container->second;
      }
    }
  }

 private:
  std::mutex buffer_map_realloc_mutex;
  mutable std::map<size_t, Image> buffer_map;


  static size_t get_thread_id() {
    size_t res;
    std::stringstream id_receiver;

    id_receiver << std::this_thread::get_id();
    id_receiver >> res;

    return res;
  }


};

class FitnessFunction {
  ImageStrokingData imageData;

  size_t total_stroke_number;
  size_t w, h;

  bool is_run_sequentially;
  mutable Image personal_buffer;

  std::shared_ptr<BufferBank> buffer_holder = std::make_shared<BufferBank>();

  bool reverse;
  Color canvas_color;

  RunTimeCounter rt_counter;

 public:
  FitnessFunction() = default;

  /**
   * @param is_run_sequentially: if it`s set to true, is stores only one buffer allocated in the constructor
   * and uses it everywhere. This approach is only applicable for sequential running!!
   */
  explicit FitnessFunction(const ImageStrokingData& imageData, size_t strokes, bool is_run_sequentially,
                           bool reverse = false, const Color& canvas_color = {0., 0., 0.});

  double operator()(const std::vector<double>& stroke_data_buffer) const;

  AlgorithmStatistics get_efficiency_account() const;
};


