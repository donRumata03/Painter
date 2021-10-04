#include <gtest/gtest.h>

#include "data/color.h"
#include "data/stroke.h"
#include "data/canvas.h"
#include "data/paint_plan.h"
#include "data/common_stroking_params.h"


TEST(JsonTest, ColorConversion) {
  ByteColor byte_color = {255, 100, 1};
  json j_byte_color = byte_color;
  auto converted_byte_color = j_byte_color.get<ByteColor>();

  ASSERT_EQ(byte_color, converted_byte_color);


  Color color = {1, 0.5, 0.25};
  json j_color = color;
  auto converted_color = j_color.get<Color>();

  ASSERT_EQ(color, converted_color);
}

TEST(JsonTest, StrokeConversion) {
  Canvas canvas(300, 400);

  Stroke stroke({1, 2}, {3, 4}, {5, 6}, 7);
  ContextWrapper<Stroke> wrapper{ stroke, canvas};
  json j_wrapper = wrapper;
  Stroke converted_stroke;
  from_json(j_wrapper, converted_stroke, canvas);

  ASSERT_EQ(stroke, converted_stroke);

  ColoredStroke col_stroke({1, 2}, {3, 4}, {5, 6}, 7);
  ContextWrapper<ColoredStroke> col_wrapper{ col_stroke, canvas};
  json j_col_wrapper = col_wrapper;
  ColoredStroke converted_col_stroke;
  from_json(j_col_wrapper, converted_col_stroke, canvas);

  ASSERT_EQ(col_stroke, converted_col_stroke);
}

TEST(JsonTest, CanvasConversion) {
  Canvas canvas(300, 400);
  json j_canvas = canvas;
  auto converted_canvas = j_canvas.get<Canvas>();

  ASSERT_EQ(canvas, converted_canvas);

}

TEST(JsonTest, PaintPlanConversion) {
  PaintPlan paint_plan = {};
  json j_paint_plan = paint_plan;
  auto converted_paint_plan = j_paint_plan.get<PaintPlan>();

  ASSERT_EQ(paint_plan, converted_paint_plan);

}

TEST(JsonTest, CommonStrokingParamsConversion) {
  CommonStrokingParams params;

  params.sequence = {GaStrokingParams{}, AnnealingStrokingParams{}};

  json j_params = params;
  auto converted_params = j_params.get<CommonStrokingParams>();

  ASSERT_EQ(params, converted_params);
}