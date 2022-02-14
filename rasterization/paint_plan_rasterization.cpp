//
// Created by Vova on 14.02.2022.
//

#include "paint_plan_rasterization.h"

#include <utils/image.h>

Image rasterize_paint_plan(const PaintPlan &plan, Color canvas_color) {
  auto image = make_default_image(
          cv::Size (int(plan.canvas.width()), int(plan.canvas.height())),
          canvas_color
          );

  rasterize_strokes(image, plan.strokes, 1000, StrokeRasterizationAlgorithm::smooth);

  return image;
}
