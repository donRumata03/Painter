#include "launch.h"

#include "data/paint_plan.h"
#include "operations/image/adaptive_params.h"
#include "operations/stroke/sorting.h"
#include "launch/workers/ga.h"
#include "launch/workers/annealing.h"
#include "launch/multizone_launcher.h"
#include "launch/vector_zone_launcher.h"
#include "utils/image.h"
#include "utils/logger.h"


static void save_log_json(const std::vector<ColoredStroke>& strokes, const Canvas& canvas = Canvas(),
                          Units units = Units::MM,
                          const fs::path& filepath = fs::path(painter_base_path) / "log" / "latest" / "plan.json") {
  json j = PaintPlan(sort_strokes(strokes, canvas.width(units), canvas.height(units)), canvas);
  std::ofstream json_file(filepath);
  json_file << j.dump(1, '\t');
  json_file.close();
  LogConsoleSuccess("Launch") << "Save paint plan to: " << filepath.string();
}

/**
 * Runs chain treating the whole image as a single zone;
 * Sequential: no multithreading
 */
static void launch_single_zone_raster(const std::string& filename, const CommonStrokingParams& params,
                                      const fs::path& logging_path) {
  Image image = open_image(filename);
  save_image(image, logging_path / "original.png");

  std::vector<ColoredStroke> strokes;
  size_t index = 0;

  // TODO: separate class for chain to remove code duplication
  // TODO: or use optimization tree
  for (auto& chain : params.sequence) {
    if (chain.index() == 0) { // GA
      LogConsoleInfo("Launch", "Single", "Raster") << "Start GA (chain " << (index + 1) << "/" << params.sequence.size()
                                                   << ")";
      GaWorker worker(image, params, std::get<GaStrokingParams>(chain),
                      logging_path / ("chain" + std::to_string(index)));
      if (!strokes.empty()) {
        worker.set_basic_strokes(get_strokes_base(strokes));
      }
      worker.run_remaining_iterations();
      worker.get_statistics().print_diagnostic_information();
      worker.save_fitness_dynamic();

      strokes = Packer::unpack_stroke_data_buffer(worker.get_best_genome());
    } else if (chain.index() == 1) { // Annealing
      LogConsoleInfo("Launch", "Single", "Raster") << "Start annealing (chain " << (index + 1) << "/"
                                                   << params.sequence.size() << ")";
      /*
      auto cur_params = params;
      Color major_color, canvas_color, background_color;
      if (auto_find_color) {
        major_color = find_major_image_color(image);
        canvas_color = find_contrast_color(major_color);
        background_color = find_image_background_color(image);

        std::cout << "[launch_single_zone_annealing]: Found major color for the image: "
                  << major_color << " the one, contrast to it, is " << canvas_color << ". It's taken to be canvas color"
                  << std::endl;
        std::cout << "[launch_single_zone_annealing]: Original Image's background color is " << background_color
                  << std::endl;

        cur_params.stroke_color = major_color;
        // this_common_params.canvas_color = canvas_color;
        cur_params.use_constant_color = true;
      }*/

      AnnealingWorker worker(image, params, std::get<AnnealingStrokingParams>(chain),
                             logging_path / ("chain" + std::to_string(index)));
      worker.run_remaining_iterations();
      worker.get_statistics().print_diagnostic_information();
      worker.save_fitness_dynamic();

      strokes = Packer::unpack_stroke_data_buffer(worker.get_best_genome());
    }

    LogConsoleSuccess("Launch", "Single", "Raster") << "Chain #" << (index + 1) << " finished";
    index++;
  }

  colorize_strokes(strokes, ImageStrokingData(image, params.use_constant_color, params.stroke_color));
  save_log_json(strokes);

  Image stroked_image = make_default_image(image.cols, image.rows, params.canvas_color);
  rasterize_strokes(stroked_image, strokes);
  save_image(stroked_image, (fs::path(painter_base_path) / "log" / "latest" / "result.png").string());
}

/**
 * — Runs `VectorZoneLauncher` in parallel manner
 * — Views result
 * — Save strokes into single json
 *
 * @param filename should be and svg file
 */
static void launch_zoned_vector_stroking(const std::string& filename, const CommonStrokingParams& params,
                                         const fs::path& logging_path) {

  VectorZoneLauncher launcher(filename, params, true);

  launcher.run();

  // View result
  // Full size, only result
  auto size = launcher.get_image_size();
  Image stroked_image = make_default_image(size.width, size.height, params.canvas_color);
  auto strokes = launcher.get_final_strokes();
  rasterize_strokes(stroked_image, strokes);
  save_image(stroked_image, (fs::path(painter_base_path) / "log" / "latest" / "result.png").string());
  LogConsoleSuccess("Launch") << "Result: " << strokes.size() << " strokes";
  std::vector<ColoredStroke> some_strokes;
  // Save strokes sequence
  for (int i = 0; i < strokes.size(); i++) {
    Image some_strokes_image = make_default_image(size.width, size.height, params.canvas_color);

    some_strokes.push_back(strokes[i]);
    rasterize_strokes(some_strokes_image, some_strokes);
    std::string image_name = std::to_string(i) + ".png";
    save_image(some_strokes_image, (latest_log_path /
                               "result_strokes" / image_name).string());

  }

  // Full size, result on canvas (in px)
  Image stroked_image_mm = make_default_image(params.canvas.width(Units::PX), params.canvas.height(Units::PX), params.canvas_color);
  rasterize_strokes(stroked_image_mm, launcher.get_final_strokes(Units::PX, true));
  save_image(stroked_image_mm, (fs::path(painter_base_path) / "log" / "latest" / "result_canvas.png").string());

  // Save strokes
  save_log_json(launcher.get_final_strokes(Units::MM, true), params.canvas);
}


void launch_stroking(const std::string& filename, const CommonStrokingParams& params, const fs::path& logging_path) {
	ensure_log_exists_and_cleared(logging_path);
  Logger::SetLogFile(logging_path / "log.txt");

  if (fs::path(filename).extension() == ".svg") {
    LogConsoleInfo("Launch") << "Filename „" << filename << "“ ends with „.svg“ => Launching SVG zoned stroking…";

    launch_zoned_vector_stroking(filename, params, logging_path);
  } else {
    LogConsoleInfo("Launch") << "Filename „" << filename << "“ does NOT end with „.svg“ => Launching raster stroking…";

    launch_single_zone_raster(filename, params, logging_path);
  }
}


/**
 * This function will probably eventually resurrect from The Oblivion…
 * It launches process of splitting a raster image into rectangular zones
 */
void launch_multizone_stroking(const std::string& filename, const CommonStrokingParams& params,
                               const fs::path& logging_path) {
  /*
    CommonStrokingParams this_common_params = default_stroking_parameters;
    GA_stroking_parameters this_ga_params = default_GA_params;
    image_splitting_params this_splitting_params = van_gogh_splitting_params;

    Image image = open_image(filename);
    MultizoneLaunchWrapper<GA_worker> launcher(image,
                                               this_splitting_params.zones_x, this_splitting_params.zones_y, this_splitting_params.overlay_percent,
                                               this_common_params, this_ga_params);
    std::cout << "[main launching function]: Performed initialization. Running.." << std::endl;

    launcher.run();

    launcher.save_result(painter_base_path / "log" / "latest" / "result.png");
   */
}