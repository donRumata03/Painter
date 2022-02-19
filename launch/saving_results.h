//
// Created by vova on 19.02.2022.
//


#pragma once

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
    Units stroke_units;

public:
    SortedStrokesForImage(double w, double h, const Color &canvas_color, const std::vector<ColoredStroke> &strokes,
                          Units stroke_units, const Canvas &canvas)
            :
            w(w), h(h),
            template_image(make_default_image(w, h, canvas_color)),
            strokes(sort_strokes(strokes, w, h)),
            stroke_units(stroke_units)

    {}

    Image get_rendered_image() {
      Image res = template_image.clone();
      rasterize_strokes(res, strokes);
      return res;
    }
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
  assert(strokes_for_image.stroke_units == Units::MM);

  auto cumulative_stroke_plans_dir = latest_log_path / "cumulative_stroke_plans";
  fs::create_directories(cumulative_stroke_plans_dir);

  std::vector<ColoredStroke> cum;
  for (int i = 0; i < strokes_for_image.strokes.size(); i++) {
    cum.push_back(strokes_for_image.strokes[i]);

    std::string plan_name = "strokes_up_to_" + std::to_string(i + 1) + ".json";
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
  assert(strokes_for_image.stroke_units == Units::MM);

  auto single_stroke_plans_dir = latest_log_path / "single_stroke_plans";
  fs::create_directories(single_stroke_plans_dir);

  for (int i = 0; i < strokes_for_image.strokes.size(); i++) {
    std::string plan_name = "stroke_" + std::to_string(i + 1) + ".json";

    save_paint_plan({ strokes_for_image.strokes[i] }, canvas, single_stroke_plans_dir / plan_name);
  }
}

