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


struct SortedStrokesForImage {
    double w = 0;
    double h = 0;
    Image template_image;
    std::vector<ColoredStroke> strokes;

public:
    SortedStrokesForImage(double w, double h, const Color &canvas_color, const std::vector<ColoredStroke> &strokes,
                          Units stroke_units, const Canvas &canvas)
            :
            w(w), h(h),
            template_image(make_default_image(w, h, canvas_color)),
            strokes(sort_strokes(strokes, w, h)),
            stroke_units(stroke_units)

            {}

//    std::vector<ColoredStroke> get_strokes_mm() {
//      return (stroke_units == Units::MM) ?
//        strokes :
//             transform_stroke_into( canvas, Units::MM)
//        ;
//    }
};


/**
 * @param strokes are assumed to be in MM!
 */
static void save_paint_plan(const std::vector<ColoredStroke>& strokes, const Canvas& canvas,
                            const fs::path& path = latest_log_path / "plan.json")
{
  json j = PaintPlan(strokes, canvas);
  std::ofstream json_file(path);
  json_file << j.dump(1, '\t');
  json_file.close();
  LogConsoleSuccess("Launch") << "Save paint plan to: " << path.string();
}


static void save_resultant_image(const SortedStrokesForImage& strokes_for_image) {
  auto temp = strokes_for_image.template_image.clone();
  rasterize_strokes(temp, strokes_for_image.strokes);
  save_image(temp, (latest_log_path / "result.png").string());
}

static void save_cumulative_stroke_images(const SortedStrokesForImage& strokes_for_image) {
  Image current_state = strokes_for_image.template_image.clone();

  for (int i = 0; i < strokes_for_image.strokes.size(); i++) {
    rasterize_stroke(current_state, strokes_for_image.strokes[i]);
    std::string image_name = "strokes_up_to_" + std::to_string(i + 1) + ".png";
    save_image(current_state, (latest_log_path / "cumulative_stroke_images" / image_name).string());
  }
}

/**
 * @param strokes_for_image strokes should be in MM
 */
static void save_cumulative_plans(const SortedStrokesForImage& strokes_for_image, const Canvas& canvas) {
  auto cumulative_stroke_plans_dir = latest_log_path / "cumulative_stroke_plans";
  fs::create_directories(cumulative_stroke_plans_dir);

  std::vector<ColoredStroke> cum;
  for (int i = 0; i < strokes_for_image.strokes.size(); i++) {
    cum.push_back(strokes_for_image.strokes[i]);

    std::string plan_name = "strokes_up_to_" + std::to_string(i + 1) + ".png";
    save_paint_plan(cum, canvas, cumulative_stroke_plans_dir / plan_name);
  }
}

static void save_single_stroke_images(const SortedStrokesForImage& strokes_for_image) {
  for (int i = 0; i < strokes_for_image.strokes.size(); i++) {
    Image with_new_stroke = strokes_for_image.template_image.clone();
    rasterize_stroke(with_new_stroke, strokes_for_image.strokes[i]);
    std::string image_name = "stroke_" + std::to_string(i + 1) + ".png";
    save_image(with_new_stroke, (latest_log_path / "single_stroke_images" / image_name).string());
  }
}


/**
 * @param strokes_for_image strokes should be in MM
 */
static void save_single_stroke_plans(const SortedStrokesForImage& strokes_for_image, const Canvas& canvas) {
  auto single_stroke_plans_dir = latest_log_path / "single_stroke_plans";
  fs::create_directories(single_stroke_plans_dir);

  for (int i = 0; i < strokes_for_image.strokes.size(); i++) {
    std::string plan_name = "stroke_" + std::to_string(i + 1) + ".png";

    save_paint_plan({ strokes_for_image.strokes[i] }, canvas, single_stroke_plans_dir / plan_name);
  }
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
  save_paint_plan(strokes);

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

  // Get result (both in Pixels on matrix, in pixels on canvas, in MM on canvas)

  auto pixel_mat_image = SortedStrokesForImage(
          launcher.get_image_size().width, launcher.get_image_size().height,
          params.canvas_color, launcher.get_final_strokes(Units::PX, false), Units::PX, <#initializer#>);

  auto pixel_canvas_image = SortedStrokesForImage(
          params.canvas.width(Units::PX), params.canvas.height(Units::PX),
          params.canvas_color, launcher.get_final_strokes(Units::PX, true), Units::PX, <#initializer#>);

  auto mm_canvas_image = SortedStrokesForImage(
          params.canvas.width(Units::MM), params.canvas.height(Units::MM),
          params.canvas_color, launcher.get_final_strokes(Units::MM, true), Units::PX, <#initializer#>);



  LogConsoleSuccess("Launch") << "Result: " << strokes_on_image.size() << " strokes";
  save_resultant_image(template_image, strokes);

  // Save cumulative stroke images with plans:
  // TODO: save into plans (take mm-s and shift into account)
  save_cumulative_stroke_images(make_default_image(image_size.width, image_size.height, params.canvas_color), strokes_on_image);

  // Single strokes with plans:
  save_single_stroke_images(make_default_image(image_size.width, image_size.height, params.canvas_color), strokes);


  // Full size, result on canvas (in px)
  Image stroked_image_mm = make_default_image(params.canvas.width(Units::PX), params.canvas.height(Units::PX), params.canvas_color);
  rasterize_strokes(stroked_image_mm, );
  save_image(stroked_image_mm, (fs::path(painter_base_path) / "log" / "latest" / "result_canvas.png").string());

  // Save strokes
  save_paint_plan(launcher.get_final_strokes(Units::MM, true), params.canvas);
}


/**
 * Decides what to run based on file extension
 */
void launch_stroking(const std::string& filename, const CommonStrokingParams& params, const fs::path& logging_path) {
	ensure_log_exists_and_cleared(logging_path);
  Logger::SetLogFile(logging_path / "Painter.log");

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