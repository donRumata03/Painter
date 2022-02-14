//
// Created by Vova on 14.02.2022.
//

#include <gtest/gtest.h>

#include <rasterization/stroke.h>
#include "data/paint_plan.h"
#include "rasterization/paint_plan_rasterization.h"

static void rasterize_plan_json(const std::string& name) {
  auto plan = json::parse(
                  *read_file(painter_base_path / "tests" / "test-plans" / (name + ".json"))
                  ).get<PaintPlan>();

  auto& strokes_mm = plan.strokes;
  std::vector<ColoredStroke> strokes_px(strokes_mm.size());
  std::transform(strokes_mm.begin(), strokes_mm.end(), strokes_px.begin(), [&](const ColoredStroke& stroke){
    return transform_stroke_into(stroke, plan.canvas, Units::PX);
  });
  plan.strokes = strokes_px;

  auto rasterized = rasterize_paint_plan(plan, { 0., 0., 0. });

  save_image(rasterized, painter_base_path / "tests" / "test-plans" / (name + ".png"));
}

TEST(Rasterizaton, Cross) {
  rasterize_plan_json("small-cross");
  rasterize_plan_json("big-cross");
}

TEST(Rasterizaton, CrossInRectangle) {
  rasterize_plan_json("small-cross-inside-rectangle");
}

